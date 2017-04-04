// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
/**
 * @class document::select::InvalidConstant
 * @ingroup select
 *
 * @brief Class describing an invalid constant in the select tree.
 *
 * @author H�kon Humberset
 * @date 2005-06-07
 * @version $Id$
 */

#pragma once

#include "node.h"

namespace document {
namespace select {

class InvalidConstant : public Node
{
public:
    explicit InvalidConstant(const vespalib::stringref &value);

    ResultList contains(const Context&) const override { return ResultList(Result::Invalid); }
    ResultList trace(const Context&, std::ostream& trace) const override;
    void print(std::ostream& out, bool verbose, const std::string& indent) const override;
    void visit(Visitor& v) const override;

    Node::UP clone() const override { return wrapParens(new InvalidConstant(_name)); }

};

} // select
} // document

