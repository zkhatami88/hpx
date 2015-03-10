//  Copyright (c) 2014 Hartmut Kaiser
//  Copyright (c) 2014 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the eighth in a series of examples demonstrating the development
// of a fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds on example seven.

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/lcos/gather.hpp>

#include <hpx/include/parallel_algorithm.hpp>
#include <boost/range/irange.hpp>

#include <boost/shared_array.hpp>
#include <boost/serialization/vector.hpp>

#include "print_time_results.hpp"

#include <stack>

///////////////////////////////////////////////////////////////////////////////
// Command-line variables
bool header = true; // print csv heading
double k = 0.5;     // heat transfer coefficient
double dt = 1.;     // time step
double dx = 1.;     // grid spacing

char const* stepper_basename = "/1d_stencil_8/stepper/";
char const* gather_basename = "/1d_stencil_8/gather/";

///////////////////////////////////////////////////////////////////////////////
struct partition_data
{
private:
    struct deleter
    {
        deleter()
          : delete_(true)
        {}

        explicit deleter(bool to_delete)
          : delete_(to_delete)
        {}

        void operator()(double * p) const
        {
            if(delete_) delete [] p;
        }

        bool delete_;
    };

public:
    partition_data()
      : size_(0)
      , min_index_(0)
      , max_index_(0)
    {}

    explicit partition_data(std::size_t size)
      : data_(new double[size]), size_(size)
      , min_index_(0)
      , max_index_(size)
    {}

    partition_data(std::size_t size, double initial_value)
      : data_(new double[size])
      , size_(size)
      , min_index_(0)
      , max_index_(size)
    {
        double base_value = double(initial_value * size);
        for (std::size_t i = 0; i != size; ++i)
            data_[i] = base_value + double(i);
    }

    partition_data(partition_data && base)
      : data_(std::move(base.data_))
      , size_(base.size())
      , min_index_(base.min_index_)
      , max_index_(base.max_index_)
    {}

    partition_data(partition_data const & base, std::size_t min_index, std::size_t max_index)
      : data_(base.data_.get() + min_index, deleter(false))
      , size_(base.size())
      , min_index_(min_index)
      , max_index_(max_index)
    {
        HPX_ASSERT(min_index < base.size());
        HPX_ASSERT(max_index <= base.size());
    }

    double& operator[](std::size_t idx)
    {
        return data_[index(idx)];
    }
    double operator[](std::size_t idx) const
    {
        return data_[index(idx)];
    }

    std::size_t size() const { return size_; }

    enum partition_type
    {
        left_partition, middle_partition, right_partition
    };

    partition_data get_data(partition_type t) const
    {
        switch (t)
        {
        case left_partition:
            return partition_data(*this, size()-1, size());

        case middle_partition:
            return partition_data(*this, 0, size());
            break;

        case right_partition:
            return partition_data(*this, 0, 1);

        default:
            HPX_ASSERT(false);
            break;
        }
    }

private:
    std::unique_ptr<double[], deleter> data_;
    std::size_t size_;
    std::size_t min_index_;
    std::size_t max_index_;

    std::size_t index(std::size_t idx) const
    {
        HPX_ASSERT(idx >= min_index_ && idx < max_index_);
        return idx - min_index_;
    }

    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class boost::serialization::access;

    template <typename Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar & min_index_ & max_index_ & size_;
        data_.reset(new double[max_index_ - min_index_]);
        boost::serialization::array<double> arr(data_.get(), max_index_ - min_index_);
        ar.load_array(arr, version);
    }

    template <typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar & min_index_ & max_index_ & size_;
        boost::serialization::array<double> arr(data_.get(), max_index_ - min_index_);
        ar.save_array(arr, version);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

std::ostream& operator<<(std::ostream& os, partition_data const& c)
{
    os << "{";
    for (std::size_t i = 0; i != c.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << c[i];
    }
    os << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
inline std::size_t idx(std::size_t i, int dir, std::size_t size)
{
    if(i == 0 && dir == -1)
        return size-1;
    if(i == size-1 && dir == +1)
        return 0;

    HPX_ASSERT((i + dir) < size);

    return i + dir;
}

///////////////////////////////////////////////////////////////////////////////
// Data for one time step on one locality
struct stepper_server : hpx::components::simple_component_base<stepper_server>
{
    // Our data for one time step
    typedef std::vector<hpx::shared_future<partition_data>> space;

    stepper_server() {}

    stepper_server(std::size_t nl)
      : left_(hpx::find_id_from_basename(stepper_basename, idx(hpx::get_locality_id(), -1, nl))),
        right_(hpx::find_id_from_basename(stepper_basename, idx(hpx::get_locality_id(), +1, nl))),
        U_(2)
    {
    }

    // do all the work on 'np' local partitions, 'nx' data points each, for
    // 'nt' time steps
    space do_work(std::size_t local_np, std::size_t nx, std::size_t nt);

    HPX_DEFINE_COMPONENT_ACTION(stepper_server, do_work, do_work_action);

    // receive the left-most partition from the right
    void from_right(std::size_t t, partition_data p)
    {
        right_receive_buffer_.store_received(t, std::move(p));
    }

    // receive the right-most partition from the left
    void from_left(std::size_t t, partition_data p)
    {
        left_receive_buffer_.store_received(t, std::move(p));
    }

    HPX_DEFINE_COMPONENT_ACTION(stepper_server, from_right, from_right_action);
    HPX_DEFINE_COMPONENT_ACTION(stepper_server, from_left, from_left_action);

protected:
    // Our operator
    static double heat(double left, double middle, double right)
    {
        return middle + (k*dt/dx*dx) * (left - 2*middle + right);
    }

    // The partitioned operator, it invokes the heat operator above on all
    // elements of a partition.
    static partition_data heat_part(hpx::shared_future<partition_data> left,
        hpx::shared_future<partition_data> middle_future, hpx::shared_future<partition_data> right_future);

    // Helper functions to receive the left and right boundary elements from
    // the neighbors.
    hpx::future<partition_data> receive_left(std::size_t t)
    {
        return left_receive_buffer_.receive(t);
    }
    hpx::future<partition_data> receive_right(std::size_t t)
    {
        return right_receive_buffer_.receive(t);
    }

    // Helper functions to send our left and right boundary elements to
    // the neighbors.
    void send_left(std::size_t t, partition_data const & p)
    {
        hpx::apply(
            from_right_action()
          , left_.get()
          , t
          , p.get_data(partition_data::left_partition)
        );
    }
    void send_right(std::size_t t, partition_data const & p)
    {
        hpx::apply(
            from_left_action()
          , right_.get()
          , t
          , p.get_data(partition_data::right_partition)
        );
    }

private:
    hpx::shared_future<hpx::id_type> left_, right_;
    std::vector<space> U_;
    hpx::lcos::local::receive_buffer<partition_data> left_receive_buffer_;
    hpx::lcos::local::receive_buffer<partition_data> right_receive_buffer_;
};

// The macros below are necessary to generate the code required for exposing
// our partition type remotely.
//
// HPX_REGISTER_MINIMAL_COMPONENT_FACTORY() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::simple_component<stepper_server> stepper_server_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(stepper_server_type, stepper_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef stepper_server::do_work_action do_work_action;
HPX_REGISTER_ACTION(do_work_action);
typedef stepper_server::from_right_action from_right_action;
HPX_REGISTER_ACTION(from_right_action);
typedef stepper_server::from_left_action from_left_action;
HPX_REGISTER_ACTION(from_left_action);

///////////////////////////////////////////////////////////////////////////////
// This is a client side member function can now be implemented as the
// stepper_server has been defined.
struct stepper : hpx::components::client_base<stepper, stepper_server>
{
    typedef hpx::components::client_base<stepper, stepper_server> base_type;

    // construct new instances/wrap existing steppers from other localities
    stepper()
      : base_type(hpx::new_<stepper_server>(hpx::find_here(), hpx::get_num_localities_sync()))
    {
        hpx::register_id_with_basename(stepper_basename, get_gid(), hpx::get_locality_id());
    }

    stepper(hpx::future<hpx::id_type> && id)
      : base_type(std::move(id))
    {}

    hpx::future<stepper_server::space> do_work(
        std::size_t local_np, std::size_t nx, std::size_t nt)
    {
        return hpx::async(do_work_action(), get_gid(), local_np, nx, nt);
    }
};

///////////////////////////////////////////////////////////////////////////////
// The partitioned operator, it invokes the heat operator above on all elements
// of a partition.
partition_data stepper_server::heat_part(hpx::shared_future<partition_data> left_future,
    hpx::shared_future<partition_data> middle_future, hpx::shared_future<partition_data> right_future)
{
    partition_data const & left = left_future.get();
    partition_data const & middle = middle_future.get();
    partition_data const & right = right_future.get();

    // All local operations are performed once the middle data of
    // the previous time step becomes available.
    std::size_t size = middle.size();
    partition_data next(size);

    next[0] = heat(left[size-1], middle[0], middle[1]);

    for (std::size_t i = 1; i != size-1; ++i)
        next[i] = heat(middle[i-1], middle[i], middle[i+1]);

    next[size-1] = heat(middle[size-2], middle[size-1], right[0]);

    return next;
}

///////////////////////////////////////////////////////////////////////////////
// This is the implementation of the time step loop
stepper_server::space stepper_server::do_work(std::size_t local_np,
    std::size_t nx, std::size_t nt)
{
    using hpx::lcos::local::dataflow;
    using hpx::util::unwrapped;

    std::vector<hpx::id_type> localities = hpx::find_all_localities();

    // U[t][i] is the state of position i at time t.
    for (space& s: U_)
        s.resize(local_np);

    // Initial conditions: f(0, i) = i
    std::size_t b = 0;
    auto range = boost::irange(b, local_np);
    using hpx::parallel::par;
    hpx::parallel::for_each(par, boost::begin(range), boost::end(range),
        [this, nx](std::size_t i)
        {
            U_[0][i] = hpx::make_ready_future(partition_data(nx, double(i)));
        }
    );
    hpx::id_type here = hpx::find_here();

    auto Op = stepper_server::heat_part;

    // send initial values to neighbors
    U_[0][0].then([this](hpx::shared_future<partition_data> p){ send_left(0, p.get()); });
    U_[0][local_np-1].then([this](hpx::shared_future<partition_data> p){ send_right(0, p.get()); });

    for (std::size_t t = 0; t != nt; ++t)
    {
        space const& current = U_[t % 2];
        space& next = U_[(t + 1) % 2];

        // handle special case (one partition per locality) in a special way
        if (local_np == 1)
        {

            next[0] = dataflow(
                    hpx::launch::async, Op,
                    receive_left(t), current[0], receive_right(t)
                );

            // send to left and right if not last time step
            if (t != nt-1)
            {
                next[0].then([this, t](hpx::shared_future<partition_data> p){ send_left(t+1, p.get()); });
                next[0].then([this, t](hpx::shared_future<partition_data> p){ send_right(t+1, p.get()); });
            }
        }
        else
        {
            next[0] = dataflow(
                    hpx::launch::async, Op,
                    receive_left(t), current[0], current[1]
                );

            // send to left if not last time step
            if (t != nt-1) next[0].then([this, t](hpx::shared_future<partition_data> p){ send_left(t + 1, p.get()); });

            for (std::size_t i = 1; i != local_np-1; ++i)
            {
                next[i] = dataflow(
                        hpx::launch::async, Op,
                        current[i-1], current[i], current[i+1]
                    );
            }

                next[local_np-1] = dataflow(
                    hpx::launch::async, Op,
                    current[local_np-2], current[local_np-1], receive_right(t)
                );

            // send to right if not last time step
            if (t != nt-1) next[local_np-1].then([this, t](hpx::shared_future<partition_data> p){ send_right(t + 1, p.get()); });
        }
    }

    return U_[nt % 2];
}

HPX_REGISTER_GATHER(stepper_server::space, stepper_server_space_gatherer);

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    using hpx::lcos::local::dataflow;

    boost::uint64_t nt = vm["nt"].as<boost::uint64_t>();   // Number of steps.
    boost::uint64_t nx = vm["nx"].as<boost::uint64_t>();   // Number of grid points.
    boost::uint64_t np = vm["np"].as<boost::uint64_t>();   // Number of partitions.

    if (vm.count("no-header"))
        header = false;

    std::vector<hpx::id_type> localities = hpx::find_all_localities();
    std::size_t nl = localities.size();                    // Number of localities

    if (np < nl)
    {
        std::cout << "The number of partitions should not be smaller than "
                     "the number of localities" << std::endl;
        return hpx::finalize();
    }

    // Create the local stepper instance, register it
    stepper step;

    // Measure execution time.
    boost::uint64_t t = hpx::util::high_resolution_clock::now();

    // Perform all work and wait for it to finish
    hpx::future<stepper_server::space> result = step.do_work(np/nl, nx, nt);

    // Gather results from all localities
    if (0 == hpx::get_locality_id())
    {
        boost::uint64_t const num_worker_threads = hpx::get_num_worker_threads();

        hpx::future<std::vector<stepper_server::space> > overall_result =
            hpx::lcos::gather_here(gather_basename, std::move(result), nl);

        std::vector<stepper_server::space> solution = overall_result.get();
        for (std::size_t i = 0; i != nl; ++i)
        {
            stepper_server::space const& s = solution[i];
            for (std::size_t i = 0; i != s.size(); ++i)
                s[i].wait();
        }

        boost::uint64_t elapsed = hpx::util::high_resolution_clock::now() - t;

        // Print the solution at time-step 'nt'.
        if (vm.count("results"))
        {
            for (std::size_t i = 0; i != nl; ++i)
            {
                stepper_server::space const& s = solution[i];
                for (std::size_t j = 0; j != np; ++j)
                {
                    std::cout << "U[" << i*np + j << "] = "
                        << s[j].get().get_data(partition_data::middle_partition)
                        << std::endl;
                }
            }
        }

        print_time_results(boost::uint32_t(nl), num_worker_threads, elapsed,
            nx, np, nt, header);
    }
    else
    {
        hpx::lcos::gather_there(gather_basename, std::move(result)).wait();
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("results", "print generated results (default: false)")
        ("nx", value<boost::uint64_t>()->default_value(10),
         "Local x dimension (of each partition)")
        ("nt", value<boost::uint64_t>()->default_value(45),
         "Number of time steps")
        ("np", value<boost::uint64_t>()->default_value(10),
         "Number of partitions")
        ("k", value<double>(&k)->default_value(0.5),
         "Heat transfer coefficient (default: 0.5)")
        ("dt", value<double>(&dt)->default_value(1.0),
         "Timestep unit (default: 1.0[s])")
        ("dx", value<double>(&dx)->default_value(1.0),
         "Local x dimension")
        ( "no-header", "do not print out the csv header row")
    ;

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> cfg;
    cfg.push_back("hpx.run_hpx_main!=1");

    return hpx::init(desc_commandline, argc, argv, cfg);
}
