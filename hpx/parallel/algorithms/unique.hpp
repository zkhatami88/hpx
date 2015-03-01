//  Copyright (c) 2015 Daniel Bourgeois
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/copy.hpp


#if !defined(HPX_PARALLEL_DETAIL_UNIQUE_MARCH_1_2015_135300)
#define HPX_PARALLEL_DETAIL_UNIQUE_MARCH_1_2015_135300

#include <hpx/hpx_fwd.hpp>
#include <hpx/parallel/execution_policy.hpp>
#include <hpx/parallel/algorithms/detail/algorithm_result.hpp>
#include <hpx/parallel/algorithms/detail/predicates.hpp>
#include <hpx/parallel/algorithms/detail/dispatch.hpp>
#include <hpx/parallel/algorithms/for_each.hpp>
#include <hpx/parallel/util/partitioner.hpp>
#include <hpx/parallel/util/scan_partitioner.hpp>
#include <hpx/parallel/util/loop.hpp>
#include <hpx/parallel/util/zip_iterator.hpp>

#include <algorithm>
#include <iterator>
#include <type_traits>

#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace hpx { namespace parallel { HPX_INLINE_NAMESPACE(v1)
{
    /////////////////////////////////////////////////////////////////////////////
    // unique_copy
    namespace detail
    {
        /// \cond NOINTERNAL
        template <typename Iter>
        struct unique_copy : public detail::algorithm<unique_copy<Iter>, Iter>
        {
            unique_copy()
              : unique_copy::algorithm("unique_copy")
            {}

            template <typename ExPolicy, typename InIter, typename Pred>
            static Iter
            sequential(ExPolicy const&, InIter first, InIter last, Iter dest,
                Pred && pred)
            {
                return std::unique_copy(first, last, dest,
                    std::forward<Pred>(pred));
            }

            template <typename ExPolicy, typename FwdIter, typename Pred>
            static typename detail::algorithm_result<ExPolicy, Iter>::type
            parallel(ExPolicy const& policy, FwdIter first, FwdIter last,
                Iter dest, Pred && pred)
            {
                typedef detail::algorithm_result<ExPolicy, Iter> result;
                std::size_t count = std::distance(first, last);
                if (count < 1)
                    return result::get(std::move(dest));
                *dest = first;

                typedef hpx::util::zip_iterator<FwdIter, FwdIter, char*>
                    zip_iterator1;
                typedef hpx::util::zip_iterator<FwdIter, char*> zip_iterator2;
                boost::shared_array<char> flags(new char[count-1]);
                std::size_t init = 1;
                FwdIter lead = first;
                FwdIter prev = lead++;

                using hpx::util::get;
                using hpx::util::make_zip_iterator;
                return util::scan_partitioner<ExPolicy, Iter,
                std::size_t>::call(
                    policy,
                    make_zip_iterator(prev, lead, flags.get()),
                    count - 1,
                    init,
                    // Flag the duplicates
                    [pred](zip_iterator1 part_begin, std::size_t part_size)
                        -> std::size_t
                    {
                        std::size_t curr = 0;
                        util::loop_n(part_begin, part_size,
                            [&pred, &curr, &prev](zip_iterator1 d) mutable
                            {
                                get<2>(*d) = pred(get<0>(*d), get<1>(*d));
                                curr += !get<2>(*d);
                            });
                        return curr;
                    },
                    // Determine how far to advance the dest iterator for each
                    // partition
                    hpx::util::unwrapped(
                        [](std::size_t const& prev, std::size_t const& curr)
                        {
                            return prev + curr;
                        }
                    ),
                    // Copy the elements into dest in parallel
                    [=](std::vector<hpx::shared_future<std::size_t> >&& r,
                        std::vector<std::size_t> const& chunk_sizes) mutable
                    {
                        return util::partitioner<ExPolicy, Iter, void>::
                        call_with_data(
                            policy,
                            hpx::util::make_zip_iterator(++first, flags.get()),
                            count - 1,
                            [dest](hpx::shared_future<std::size_t>&& pos,
                                zip_iterator2 part_begin,std::size_t part_count)
                            {
                                Iter iter = dest;
                                std::advance(iter, pos.get());
                                util::loop_n(part_begin, part_count,
                                [&iter](zip_iterator2 d)
                                {
                                    if(!hpx::util::get<1>(*d))
                                        *iter++ = hpx::util::get<0>(*d);
                                });
                            },
                            [=](std::vector<hpx::future<void> >&&) mutable
                                -> Iter
                            {
                                std::advance(dest, r[r.size()-1].get());
                                return dest;
                            },
                            chunk_sizes,
                            std::move(r)
                        );
                    }
                );
            }
        };
        /// \endcond
    }

    /// Copies the elements in the range, defined by [first, last), to another
    /// range beginning at \a dest. Copies only the elements for which the
    /// predicate \a f returns true. The order of the elements that are not
    /// removed is preserved.
    ///
    /// \note   Complexity: Performs not more than \a last - \a first
    ///         assignments, exactly \a last - \a first applications of the
    ///         predicate \a f.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the assignments.
    /// \tparam InIter      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam OutIter     The type of the iterator representing the
    ///                     destination range (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     output iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a copy_if requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param dest         Refers to the beginning of the destination range.
    /// \param f            Specifies the function (or function object) which
    ///                     will be invoked for each of the elements in the
    ///                     sequence specified by [first, last).This is an
    ///                     unary predicate which returns \a true for the
    ///                     required elements. The signature of this predicate
    ///                     should be equivalent to:
    ///                     \code
    ///                     bool pred(const Type &a);
    ///                     \endcode \n
    ///                     The signature does not need to have const&, but
    ///                     the function must not modify the objects passed to
    ///                     it. The type \a Type must be such that an object of
    ///                     type \a InIter can be dereferenced and then
    ///                     implicitly converted to Type.
    ///
    /// The assignments in the parallel \a copy_if algorithm invoked with
    /// an execution policy object of type \a sequential_execution_policy
    /// execute in sequential order in the calling thread.
    ///
    /// The assignments in the parallel \a copy_if algorithm invoked with
    /// an execution policy object of type \a parallel_execution_policy or
    /// \a parallel_task_execution_policy are permitted to execute in an unordered
    /// fashion in unspecified threads, and indeterminately sequenced
    /// within each thread.
    ///
    /// \returns  The \a copy_if algorithm returns a \a hpx::future<OutIter> if the
    ///           execution policy is of type
    ///           \a sequential_task_execution_policy or
    ///           \a parallel_task_execution_policy and
    ///           returns \a OutIter otherwise.
    ///           The \a copy_if algorithm returns the output iterator to the
    ///           element in the destination range, one past the last element
    ///           copied.
    ///
    template
        <typename ExPolicy, typename InIter, typename OutIter, typename Pred>
        inline typename boost::enable_if<
            is_execution_policy<ExPolicy>,
            typename detail::algorithm_result<ExPolicy, OutIter>::type
        >::type
    unique_copy(ExPolicy&& policy, InIter first, InIter last, OutIter dest,
        Pred && pred)
    {
        typedef typename std::iterator_traits<InIter>::iterator_category
            input_iterator_category;
        typedef typename std::iterator_traits<OutIter>::iterator_category
            output_iterator_category;

        BOOST_STATIC_ASSERT_MSG(
            (boost::is_base_of<
                std::input_iterator_tag, input_iterator_category>::value),
            "Required at least input iterator.");

        BOOST_STATIC_ASSERT_MSG(
            (boost::mpl::or_<
                boost::is_base_of<
                    std::forward_iterator_tag, output_iterator_category>,
                boost::is_same<
                    std::output_iterator_tag, output_iterator_category>
            >::value),
            "Requires at least output iterator.");

        typedef typename boost::mpl::or_<
            is_sequential_execution_policy<ExPolicy>,
            boost::is_same<std::input_iterator_tag, input_iterator_category>,
            boost::is_same<std::output_iterator_tag, output_iterator_category>
        >::type is_seq;

        return detail::unique_copy<OutIter>().call(
            std::forward<ExPolicy>(policy), is_seq(),
            first, last, dest, std::forward<Pred>(pred));
    }

    //DOCUMENTATION GOES HERE

    template
        <typename ExPolicy, typename InIter, typename OutIter>
        inline typename boost::enable_if<
            is_execution_policy<ExPolicy>,
            typename detail::algorithm_result<ExPolicy, OutIter>::type
        >::type
    unique_copy(ExPolicy&& policy, InIter first, InIter last, OutIter dest)
    {
        typedef typename std::iterator_traits<InIter>::iterator_category
            input_iterator_category;
        typedef typename std::iterator_traits<OutIter>::iterator_category
            output_iterator_category;
        typedef typename std::iterator_traits<InIter>::value_type
            value_type;

        BOOST_STATIC_ASSERT_MSG(
            (boost::is_base_of<
                std::input_iterator_tag, input_iterator_category>::value),
            "Required at least input iterator.");

        BOOST_STATIC_ASSERT_MSG(
            (boost::mpl::or_<
                boost::is_base_of<
                    std::forward_iterator_tag, output_iterator_category>,
                boost::is_same<
                    std::output_iterator_tag, output_iterator_category>
            >::value),
            "Requires at least output iterator.");

        typedef typename boost::mpl::or_<
            is_sequential_execution_policy<ExPolicy>,
            boost::is_same<std::input_iterator_tag, input_iterator_category>,
            boost::is_same<std::output_iterator_tag, output_iterator_category>
        >::type is_seq;

        return detail::unique_copy<OutIter>().call(
            std::forward<ExPolicy>(policy), is_seq(),
            first, last, dest, std::equal_to<value_type>());
    }
}}}

#endif
