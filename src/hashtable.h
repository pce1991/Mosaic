#include <string.h>

typedef uint32 KeyHash(void *key);

// @WARNING: if the key is padded, the hash will not be the same between two different instances of it
inline uint32 Hash(void *key, uint32 keyLength) {
    uint32 result = 0;

    for (int i = 0; i < keyLength; i++) {
        result = result * 33 + ((uint8 *)key)[i];
    }

    return result;
}

inline uint32 ProbeLength(uint32 capacity, uint32 hash, uint32 slotIndex) {
    uint32 preferredSlot = hash % capacity;
    uint32 probeLength = (slotIndex - preferredSlot + capacity) % capacity;
    return probeLength;
}


template <typename K, typename V>
struct HashTable {
    uint32 capacity;
    uint32 count;

    bool *occupied;
    uint32 *hashes;
    K *keys;
    uint32 *keyLengths;

    V *values;
};

template <typename K, typename V>
void AllocateHashTable(HashTable<K, V> *table, uint32 capacity) {
    table->capacity = capacity;
    table->count = 0;

    table->hashes = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->occupied = (bool *)malloc(sizeof(bool) * capacity);
    table->keys = (K *)malloc(sizeof(K) * capacity);
    table->keyLengths = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->values = (V *)malloc(sizeof(V) * capacity);


    memset(table->hashes, sizeof(uint32) * capacity);
    memset(table->occupied, sizeof(bool) * capacity);
    memset(table->keys, sizeof(K) * capacity);
    memset(table->keyLengths , sizeof(uint32) * capacity);
    memset(table->values , sizeof(V) * capacity);
}

template <typename K, typename V>
void AllocateHashTable(HashTable<K, V> *table, uint32 capacity, MAllocator *allocator) {
    table->capacity = capacity;
    table->count = 0;

    table->hashes = (uint32 *)allocator->allocate(allocator, sizeof(uint32) * capacity);
    table->occupied = (bool *)allocator->allocate(allocator, sizeof(bool) * capacity);
    table->keys = (K *)allocator->allocate(allocator, sizeof(K) * capacity);
    table->keyLengths = (uint32 *)allocator->allocate(allocator, sizeof(uint32) * capacity);
    table->values = (V *)allocator->allocate(allocator, sizeof(V) * capacity);


    memset(table->hashes, sizeof(uint32) * capacity);
    memset(table->occupied, sizeof(bool) * capacity);
    memset(table->keys, sizeof(K) * capacity);
    memset(table->keyLengths , sizeof(uint32) * capacity);
    memset(table->values , sizeof(V) * capacity);
}

template <typename K, typename V>
void DeallocateHashTable(HashTable<K, V> *table) {
    free(table->hashes);
    free(table->occupied);
    free(table->keys);
    free(table->keyLengths);
    free(table->values);
}

template <typename K, typename V>
HashTable<K, V>CopyHashTable(HashTable<K, V> *table) {
    HashTable<K, V> copy = {};
    AllocateHashTable<K, V>(&copy, table->capacity);
    copy.count = table->count;

    memcpy(copy.hashes, table->hashes, sizeof(uint32) * table->capacity);
    memcpy(copy.occupied, table->occupied, sizeof(bool) * table->capacity);
    memcpy(copy.keys, table->keys, sizeof(K) * table->capacity);
    memcpy(copy.keyLengths, table->keyLengths, sizeof(uint32) * table->capacity);
    memcpy(copy.values, table->values, sizeof(V) * table->capacity);

    return copy;
}

template <typename K, typename V>
void ResizeHashTable(HashTable<K *, V> *table, uint32 capacity) {
    if (capacity <= table->capacity) {
        return;
    }

    uint32 oldCapacity = table->capacity;

    table->capacity = capacity;
    table->count = 0;

    uint32 *oldHashes = table->hashes;
    bool *oldOccupied = table->occupied;
    K **oldKeys = table->keys;
    uint32 *oldKeyLengths = table->keyLengths;
    V *oldValues = table->values;

    table->hashes = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->occupied = (bool *)malloc(sizeof(bool) * capacity);
    table->keys = (K **)malloc(sizeof(K *) * capacity);
    table->keyLengths  = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->values  = (V *)malloc(sizeof(V) * capacity);

    memset(table->hashes, sizeof(uint32) * capacity);
    memset(table->occupied, sizeof(bool) * capacity);
    memset(table->keys, sizeof(K) * capacity);
    memset(table->keyLengths , sizeof(uint32) * capacity);
    memset(table->values , sizeof(V) * capacity);

    for (int i = 0; i < oldCapacity; i++) {
        bool occupied = oldOccupied[i];
        K *key = oldKeys[i];
        uint32 keyLength = oldKeyLengths[i];
        V value = oldValues[i];

        if (occupied) {
            HashTableInsert(table, key, keyLength, value);
        }
    }

    free(oldHashes);
    free(oldOccupied);
    free((void *)oldKeys);
    free(oldKeyLengths);
    free(oldValues);
}

template <typename K, typename V>
void ResizeHashTable(HashTable<K, V> *table, uint32 capacity) {
    if (capacity <= table->capacity) {
        return;
    }

    uint32 oldCapacity = table->capacity;

    table->capacity = capacity;
    table->count = 0;

    uint32 *oldHashes = table->hashes;
    bool *oldOccupied = table->occupied;
    K *oldKeys = table->keys;
    uint32 *oldKeyLengths = table->keyLengths;
    V *oldValues = table->values;

    table->hashes = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->occupied = (bool *)malloc(sizeof(bool) * capacity);
    table->keys = (K *)malloc(sizeof(K) * capacity);
    table->keyLengths  = (uint32 *)malloc(sizeof(uint32) * capacity);
    table->values  = (V *)malloc(sizeof(V) * capacity);

    memset(table->hashes, sizeof(uint32) * capacity);
    memset(table->occupied, sizeof(bool) * capacity);
    memset(table->keys, sizeof(K) * capacity);
    memset(table->keyLengths , sizeof(uint32) * capacity);
    memset(table->values , sizeof(V) * capacity);

    for (int i = 0; i < oldCapacity; i++) {
        bool occupied = oldOccupied[i];
        K key = oldKeys[i];
        uint32 keyLength = oldKeyLengths[i];
        V value = oldValues[i];

        if (occupied) {
            HashTableInsert(table, key, value);
        }
    }

    free(oldHashes);
    free(oldOccupied);
    free(oldKeys);
    free(oldKeyLengths);
    free(oldValues);
}

template <typename K, typename V>
void HashTableClear(HashTable<K, V> *table) {
    memset(table->hashes, sizeof(uint32) * table->capacity);
    memset(table->occupied, sizeof(bool) * table->capacity);
    memset(table->keys, sizeof(K) * table->capacity);
    memset(table->keyLengths , sizeof(uint32) * table->capacity);
    memset(table->values , sizeof(V) * table->capacity);

    table->count = 0;
}

template <typename K, typename V>
void HashTableResizeIfNeeded_(HashTable<K, V> *table) {
    if (table->count == table->capacity) {
        uint32 newCapacity = Max(1u, 2 * table->capacity);
        ResizeHashTable(table, newCapacity);
    }
}

template <typename K, typename V>
inline bool HashTableMatchesAtSlot_(HashTable<K *, V> *table, uint32 slotIndex, K *key, uint32 keyLength, uint32 hash) {
    if (!table->occupied[slotIndex]) {
        return false;
    }

    if (hash != table->hashes[slotIndex]) {
        return false;
    }

    uint32 slotKeyLength = table->keyLengths[slotIndex];
    K *slotKey = table->keys[slotIndex];

    if (slotKeyLength != keyLength) {
        return false;
    }

    // Because our keyLength is the number of elements in the buffer we need to scale
    // by the type size. 
    u64 typeSize = sizeof(K);
    return keyLength == 0 || memcmp(slotKey, key, keyLength * typeSize) == 0;
}

// keyA is the key in the map
typedef bool KeyComparator(void *keyA, void *keyB, uint32 keyLength);

template <typename K, typename V>
inline bool HashTableMatchesAtSlot_(HashTable<K, V> *table, uint32 slotIndex, K key, uint32 keyLength, uint32 hash, KeyComparator *Comparator) {
    if (!table->occupied[slotIndex]) {
        return false;
    }

    if (hash != table->hashes[slotIndex]) {
        return false;
    }

    uint32 slotKeyLength = table->keyLengths[slotIndex];
    K slotKey = table->keys[slotIndex];

    if (slotKeyLength != keyLength) {
        return false;
    }

    return keyLength == 0 || Comparator(&slotKey, &key, keyLength);
}

template <typename K, typename V>
inline bool HashTableMatchesAtSlot_(HashTable<K, V> *table, uint32 slotIndex, K key, uint32 keyLength, uint32 hash) {
    if (!table->occupied[slotIndex]) {
        return false;
    }

    if (hash != table->hashes[slotIndex]) {
        return false;
    }

    uint32 slotKeyLength = table->keyLengths[slotIndex];
    K slotKey = table->keys[slotIndex];

    if (slotKeyLength != keyLength) {
        return false;
    }

    // @NOTE: keyLength is really size of the key in bytes. 
    return keyLength == 0 || memcmp(&slotKey, &key, keyLength) == 0;
}


template <typename K, typename V>
void HashTableInsert_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash, V value, bool canGrow = false) {
    if (canGrow) {
        HashTableResizeIfNeeded_(table);
    }

    ASSERT(table->capacity > 0);
    ASSERT(table->count < table->capacity);

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    while (true) {
        bool match = HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash);

        if (match || !table->occupied[slotIndex]) {
            table->occupied[slotIndex] = true;
            table->hashes[slotIndex] = hash;
            table->keys[slotIndex] = key;
            table->keyLengths[slotIndex] = keyLength;
            table->values[slotIndex] = value;

            if (!match) {
                table->count++;    
            }
            break;
        }


        ASSERT(table->occupied[slotIndex]);

        // @TODO: maybe swap, robin hood style

        slotIndex = (slotIndex + 1) % table->capacity;
        if (slotIndex == initialSlot) {
            break;
        }
    }
}

template <typename K, typename V>
void HashTableInsert_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash, V value, KeyComparator *Comparator, bool canGrow = false) {
    if (canGrow) {
        HashTableResizeIfNeeded_(table);
    }

    ASSERT(table->capacity > 0);
    ASSERT(table->count < table->capacity);

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    while (true) {
        bool match = HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash, Comparator);

        if (match || !table->occupied[slotIndex]) {
            table->occupied[slotIndex] = true;
            table->hashes[slotIndex] = hash;
            table->keys[slotIndex] = key;
            table->keyLengths[slotIndex] = keyLength;
            table->values[slotIndex] = value;

            if (!match) {
                table->count++;    
            }
            break;
        }


        ASSERT(table->occupied[slotIndex]);

        // @TODO: maybe swap, robin hood style

        slotIndex = (slotIndex + 1) % table->capacity;
        if (slotIndex == initialSlot) {
            break;
        }
    }
}


template <typename K, typename V>
void HashTableInsert(HashTable<K *, V> *table, K *key, uint32 keyLength, V value, bool canGrow = false) {
    uint32 hash = Hash((void *)key, keyLength);
    HashTableInsert_(table, key, keyLength, hash, value, canGrow);
}

template <typename V>
void HashTableInsert(HashTable<const char *, V> *table, const char *key, V value, bool canGrow = false) {
    ASSERT(key != NULL);

    uint32 keyLength = strlen(key);
    HashTableInsert(table, key, keyLength, value, canGrow);
}

template <typename K, typename V>
void HashTableInsert(HashTable<K, V> *table, K key, V value, bool canGrow = false) {
    uint32 hash = Hash(&key, sizeof(K));
    HashTableInsert_(table, key, sizeof(K), hash, value, canGrow);
}


template <typename K, typename V>
void HashTableInsert(HashTable<K, V> *table, K key, V value, KeyComparator *Comparator, bool canGrow = false) {
    uint32 hash = Hash(&key, sizeof(K));
    HashTableInsert_(table, key, sizeof(K), hash, value, Comparator, canGrow);
}

template <typename K, typename V>
void HashTableInsert(HashTable<K, V> *table, K key, KeyHash *HashFunc, KeyComparator *Comparator, V value, bool canGrow = false) {
    uint32 hash = HashFunc(&key);
    HashTableInsert_(table, key, sizeof(K), hash, value, Comparator, canGrow);
}

template <typename K, typename V>
void HashTableInsert(HashTable<K, V> *table, K key, uint32 hash, KeyComparator *Comparator, V value, bool canGrow = false) {
    HashTableInsert_(table, key, sizeof(K), hash, value, Comparator, canGrow);
}

template <typename K, typename V>
bool HashTableGet_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash, V *outValue = NULL, uint32 *outSlotIndex = NULL) {
    if (table->count == 0) {
        return false;
    }

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    while (true) {
        if (!table->occupied[slotIndex]) {
            return false;
        }

        if (HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash)) {
            if (outValue != NULL) {
                *outValue = table->values[slotIndex];
            }

            if (outSlotIndex != NULL) {
                *outSlotIndex = slotIndex;
            }

            return true;
        }

        slotIndex = (slotIndex + 1) % table->capacity;
        if (slotIndex == initialSlot) {
            break;
        }
    }

    return false;
}


template <typename K, typename V>
bool HashTableGet_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash, KeyComparator *Comparator, V *outValue = NULL, uint32 *outSlotIndex = NULL) {
    if (table->count == 0) {
        return false;
    }

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    while (true) {
        if (!table->occupied[slotIndex]) {
            return false;
        }

        if (HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash, Comparator)) {
            if (outValue != NULL) {
                *outValue = table->values[slotIndex];
            }

            if (outSlotIndex != NULL) {
                *outSlotIndex = slotIndex;
            }

            return true;
        }

        slotIndex = (slotIndex + 1) % table->capacity;
        if (slotIndex == initialSlot) {
            break;
        }
    }

    return false;
}


template <typename K, typename V>
bool HashTableGetKeyPtr_(HashTable<K *, V> *table, K *key, uint32 keyLength, uint32 hash, V *outValue = NULL, uint32 *outSlotIndex = NULL) {
    if (table->count == 0) {
        return false;
    }

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    while (true) {
        if (!table->occupied[slotIndex]) {
            return false;
        }

        if (HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash)) {
            if (outValue != NULL) {
                *outValue = table->values[slotIndex];
            }

            if (outSlotIndex != NULL) {
                *outSlotIndex = slotIndex;
            }

            return true;
        }

        slotIndex = (slotIndex + 1) % table->capacity;
        if (slotIndex == initialSlot) {
            break;
        }
    }

    return false;
}


template <typename K, typename V>
bool HashTableGet(HashTable<K *, V> *table, K *key, uint32 keyLength, V *value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    uint32 hash = Hash((void *)key, keyLength);
    return HashTableGetKeyPtr_(table, key, keyLength, hash, value, slotIndex);
}

template <typename V>
bool HashTableGet(HashTable<const char *, V> *table, const char *key, V *value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    uint32 keyLength = strlen(key);
    return HashTableGet(table, key, keyLength, value, slotIndex);
}

template <typename V>
bool HashTableGet(HashTable<char *, V> *table, char *key, V *value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    uint32 keyLength = strlen(key);
    return HashTableGet(table, key, keyLength, value, slotIndex);
}

template <typename K, typename V>
bool HashTableGet(HashTable<K, V> *table, K key, V *value, uint32 *slotIndex = NULL) {
    uint32 hash = Hash(&key, sizeof(K));
    return HashTableGet_(table, key, sizeof(K), hash, value, slotIndex);
}


template <typename K, typename V>
bool HashTableGet(HashTable<K, V> *table, K key, KeyComparator *Comparator, V *value, uint32 *slotIndex = NULL) {
    uint32 hash = Hash(&key, sizeof(K));
    return HashTableGet_(table, key, sizeof(K), hash, Comparator, value, slotIndex);
}

template <typename K, typename V>
bool HashTableGet(HashTable<K, V> *table, K key, KeyHash *HashFunc, KeyComparator *Comparator, V *value, uint32 *slotIndex = NULL) {
    uint32 hash = HashFunc(&key);
    return HashTableGet_(table, key, sizeof(K), hash, Comparator, value, slotIndex);
}

template <typename K, typename V>
bool HashTableGet(HashTable<K, V> *table, K key, uint32 hash, KeyComparator *Comparator, V *value, uint32 *slotIndex = NULL) {
    return HashTableGet_(table, key, sizeof(K), hash, Comparator, value, slotIndex);
}

template <typename K, typename V>
bool HashTableGetPtr_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash, V **outValue = NULL, uint32 *outSlotIndex = NULL) {
    if (table->count == 0) {
        return false;
    }
    
    uint32 index;
    if (HashTableGet_(table, key, keyLength, hash, (V *)NULL, &index)) {

        *outValue = &table->values[index];

        if (outSlotIndex != NULL) {
            *outSlotIndex = index;
        }

        return true;
    }

    return false;
}

template <typename K, typename V>
bool HashTableGetPtr(HashTable<K *, V> *table, K *key, uint32 keyLength, V **value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    uint32 hash = Hash((void *)key, keyLength);
    return HashTableGetPtr_(table, key, keyLength, hash, value, slotIndex);
}

template <typename V>
bool HashTableGetPtr(HashTable<const char *, V> *table, const char *key, V **value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    uint32 keyLength = strlen(key);
    return HashTableGetPtr(table, key, keyLength, value);
}

template <typename V>
bool HashTableGetPtr(HashTable<const char *, V> *table, const char *key, char keyLength, V **value, uint32 *slotIndex = NULL) {
    ASSERT(key != NULL);
    return HashTableGetPtr(table, key, keyLength, value);
}

template <typename K, typename V>
bool HashTableGetPtr(HashTable<K, V> *table, K key, V **value, uint32 *slotIndex = NULL) {
    uint32 hash = Hash(&key, sizeof(K));
    return HashTableGetPtr_(table, key, sizeof(K), hash, value, slotIndex);
}


template <typename K, typename V>
bool HashTableContains(HashTable<K *, V> *table, K *key, uint32 keyLength) {
    return HashTableGet(table, key, keyLength, (V *)NULL);
}

template <typename K, typename V>
bool HashTableContains(HashTable<const char*, V> *table, const char *key) {
    return HashTableGet(table, key, (const char *)NULL);
}

template <typename K, typename V>
bool HashTableContains(HashTable<K, V> *table, K key) {
    return HashTableGet(table, key, (V *)NULL);
}


template <typename K, typename V>
void HashTableCompressProbeList_(HashTable<K, V> *table, uint32 slotIndex, uint32 probeLength) {
    uint32 swapIndex = slotIndex;
        
    while (true) {
        slotIndex = (slotIndex + 1) % table->capacity;
        probeLength++;

        if (slotIndex == swapIndex) {
            break;
        }

        if (!table->occupied[slotIndex]) {
            break;
        }

        if (ProbeLength(table->capacity, table->hashes[slotIndex], slotIndex) >= probeLength) {
            table->occupied[swapIndex] = true;
            table->hashes[swapIndex] = table->hashes[slotIndex];
            table->keys[swapIndex] = table->keys[slotIndex];
            table->keyLengths[swapIndex] = table->keyLengths[slotIndex];
            table->values[swapIndex] = table->values[slotIndex];

            swapIndex = slotIndex;
            table->occupied[slotIndex] = false;
            probeLength = 0;
        }
    }
}

template <typename K, typename V>
bool HashTableRemove_(HashTable<K, V> *table, K key, uint32 keyLength, uint32 hash) {
    if (table->count == 0) {
        return false;
    }

    uint32 initialSlot = hash % table->capacity;
    uint32 slotIndex = initialSlot;

    uint32 probeLength = 0;

    while (true) {
        if (HashTableMatchesAtSlot_(table, slotIndex, key, keyLength, hash)) {
            table->occupied[slotIndex] = false;
            HashTableCompressProbeList_(table, slotIndex, probeLength);
            table->count--;
            return true;
        }

        slotIndex = (slotIndex + 1) % table->capacity;
        probeLength++;
        if (slotIndex == initialSlot) {
            break;
        }
    }

    return false;
}

template <typename K, typename V>
bool HashTableRemove(HashTable<K *, V> *table, K *key, uint32 keyLength) {
    ASSERT(key != NULL);
    uint32 hash = Hash((void *)key, keyLength);
    return HashTableRemove_(table, key, keyLength, hash);
}

template <typename V>
bool HashTableRemove(HashTable<const char *, V> *table, const char *key) {
    ASSERT(key != NULL);
    return HashTableRemove(table, key, strlen(key));
}

template <typename K, typename V>
bool HashTableRemove(HashTable<K, V> *table, K key) {
    uint32 hash = Hash(&key, sizeof(K));
    return HashTableRemove_(table, key, sizeof(K), hash);
}
