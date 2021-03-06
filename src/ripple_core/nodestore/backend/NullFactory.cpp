//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

namespace ripple {
namespace NodeStore {

class NullBackend : public Backend
{
public:
    explicit NullBackend (Scheduler& scheduler)
        : m_scheduler (scheduler)
    {
    }

    ~NullBackend ()
    {
    }

    std::string getName()
    {
        return std::string ();
    }

    Status fetch (void const*, NodeObject::Ptr*)
    {
        return notFound;
    }
    
    void store (NodeObject::ref object)
    {
    }
    
    void storeBatch (Batch const& batch)
    {
    }

    void visitAll (VisitCallback& callback)
    {
    }

    int getWriteLoad ()
    {
        return 0;
    }

private:
    Scheduler& m_scheduler;
};

//------------------------------------------------------------------------------

class NullFactory : public Factory
{
public:
    String getName () const
    {
        return "none";
    }

    std::unique_ptr <Backend> createInstance (
        size_t, Parameters const&, Scheduler& scheduler, Journal journal)
    {
        return std::make_unique <NullBackend> (scheduler);
    }
};

//------------------------------------------------------------------------------

std::unique_ptr <Factory> make_NullFactory ()
{
    return std::make_unique <NullFactory> ();
}

}
}
