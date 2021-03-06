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

class FetchPackTests : public UnitTest
{
public:
    enum
    {
        tableItems = 100,
        tableItemsExtra = 20
    };

    typedef boost::unordered_map <uint256, Blob> Map;

    struct TestFilter : SHAMapSyncFilter
    {
        TestFilter (Map& map, Journal journal) : mMap (map), mJournal (journal)
        {
        }

        void gotNode (bool fromFilter,
            SHAMapNode const& id, uint256 const& nodeHash,
                Blob& nodeData, SHAMapTreeNode::TNType type)
        {
        }

        bool haveNode (SHAMapNode const& id,
            uint256 const& nodeHash, Blob& nodeData)
        {
            Map::iterator it = mMap.find (nodeHash);
            if (it == mMap.end ())
            {
                mJournal.fatal << "Test filter missing node";
                return false;
            }
            nodeData = it->second;
            return true;
        }

        Map& mMap;
        Journal mJournal;
    };


    void on_fetch (Map& map, uint256 const& hash, Blob const& blob)
    {
        Serializer s (blob);
        expect (s.getSHA512Half() == hash, "Hash mismatch");
        map.emplace (hash, blob);
    }

    void runTest ()
    {
        using namespace RadixMap;

        beginTestCase ("Build");

        boost::shared_ptr <Table> t1 (boost::make_shared <Table> (smtFREE));
        add_random_items (tableItems, *t1, random());
        boost::shared_ptr <Table> t2 (t1->snapShot (true));
        
        add_random_items (tableItemsExtra, *t1, random ());
        add_random_items (tableItemsExtra, *t2, random ());

        // turn t1 into t2
        Map map;
        t2->getFetchPack (t1.get(), true, 1000000, boost::bind (
            &FetchPackTests::on_fetch, this, boost::ref (map), _1, _2));
        t1->getFetchPack (nullptr, true, 1000000, boost::bind (
            &FetchPackTests::on_fetch, this, boost::ref (map), _1, _2));

        // try to rebuild t2 from the fetch pack
        boost::shared_ptr <Table> t3;
        try
        {
            TestFilter filter (map, journal ());

            t3 = boost::make_shared <Table> (smtFREE, t2->getHash () );

            expect (t3->fetchRoot (t2->getHash (), &filter), "unable to get root");

            // everything should be in the pack, no hashes should be needed
            std::vector <uint256> hashes = t3->getNeededHashes(1, &filter);
            expect (hashes.empty(), "missing hashes");
        }
        catch (...)
        {
            failException ();
        }

        expect (t3->getHash () == t2->getHash (), "root hashes do not match");
        expect (t3->deepCompare (*t2), "failed compare");

        pass ();
    }

    FetchPackTests () : UnitTest ("FetchPack", "ripple")
    {
    }
};

static FetchPackTests fetchPackTests;

}
