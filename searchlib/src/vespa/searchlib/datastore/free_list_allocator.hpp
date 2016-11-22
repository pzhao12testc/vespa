// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "free_list_allocator.h"
#include "bufferstate.h"

namespace search {
namespace datastore {

template <typename EntryT, typename RefT, typename ReclaimerT>
FreeListAllocator<EntryT, RefT, ReclaimerT>::FreeListAllocator(DataStoreBase &store, uint32_t typeId)
    : ParentType(store, typeId)
{
}

namespace allocator {

template <typename EntryT, typename ... Args>
struct Assigner {
    static void assign(EntryT &entry, Args && ... args) {
        entry = EntryT(std::forward<Args>(args)...);
    }
};

template <typename EntryT>
struct Assigner<EntryT> {
    static void assign(EntryT &entry) {
        (void) entry;
    }
};

// Assignment operator
template <typename EntryT>
struct Assigner<EntryT, const EntryT &> {
    static void assign(EntryT &entry, const EntryT &rhs) {
        entry = rhs;
    }
};

// Move assignment
template <typename EntryT>
struct Assigner<EntryT, EntryT &&> {
    static void assign(EntryT &entry, EntryT &&rhs) {
        entry = std::move(rhs);
    }
};

}

template <typename EntryT, typename RefT, typename ReclaimerT>
template <typename ... Args>
typename Allocator<EntryT, RefT>::HandleType
FreeListAllocator<EntryT, RefT, ReclaimerT>::alloc(Args && ... args)
{
    BufferState::FreeListList &freeListList = _store.getFreeList(_typeId);
    if (freeListList._head == NULL) {
        return ParentType::alloc(std::forward<Args>(args)...);
    }
    BufferState &state = *freeListList._head;
    assert(state.isActive());
    RefT ref = state.popFreeList();
    EntryT *entry = _store.template getBufferEntry<EntryT>(ref.bufferId(), ref.offset());
    ReclaimerT::reclaim(entry);
    allocator::Assigner<EntryT, Args...>::assign(*entry, std::forward<Args>(args)...);
    return HandleType(ref, entry);
}

}
}
