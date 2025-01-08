/*
 * opencog/util/lazy_selector.cc
 *
 * Copyright (C) 2002-2009 Novamente LLC
 * All Rights Reserved
 *
 * Authors Moshe Looks, Nil Geisweiller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "lazy_selector.h"
#include "exceptions.h"
#include "oc_assert.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>
#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>

namespace opencog
{

lazy_selector::lazy_selector(unsigned int u, unsigned int l)
    : _u(u), _l(l)
{
    OC_ASSERT(u - l > 0, "you cannot select any thing from an empty list");
}

bool lazy_selector::empty() const
{
    return _l >= _u;
}

unsigned int lazy_selector::count_n_free() const
{
    return std::count_if(
                    boost::counting_iterator<unsigned int>(_l),
                    boost::counting_iterator<unsigned int>(_u),
                    boost::bind(&lazy_selector::is_free, this, boost::placeholders::_1));
}

void lazy_selector::reset_range(unsigned int new_n) {
    cassert(new_n > 0, "lazy_selector - n must be > 0.");
    n = new_n;
    _selected_indices.clear();
}

void lazy_selector::reset_range(unsigned int new_u, unsigned int new_l)
{
	OC_ASSERT(new_l >= _l,
	          "You cannot reset the lower bound by a lower number, "
	          "due to the workings of the algorithm.");
    _u = new_u;
    _l = new_l;
}

/**
 * returns the selected number (never twice the same)
 *
 * It works by creating a mapping between already selected indices and
 * free indices (never selected yet).
 *
 * To chose the free indices, we use an index _l, that
 * goes from the lower range to the upper range and only corresponds to
 * free indices. So every time _l corresponds to a non free index
 * it is incremented until it gets to a free index.
 *
 */
unsigned int lazy_selector::operator()()
{
    cassert(_selected_indices.size() < n, 
            "lazy_selector - All elements have been selected.");
    unsigned int sel_idx = select();
    _selected_indices.insert(sel_idx);
    return sel_idx;
}

bool lazy_selector::is_free(unsigned int idx) const
{
    return _picked.find(idx) == _picked.end();
}

void lazy_selector::increase_l_till_free()
{
    do {
        _l++;
    } while(!is_free(_l));
}

bool lazy_selector::is_still_free(unsigned int idx) const {
    cassert(idx < n, "lazy_selector - Index greater than n.");
    return _selected_indices.find(idx) == _selected_indices.end();
}

} //~namespace opencog
