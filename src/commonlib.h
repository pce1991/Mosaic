
#if _WIN32
#undef FIELD_OFFSET
#endif

#define FIELD_OFFSET(type, field) ((uint64)&(((type *)NULL)->field))

#define POINTER_OFFSET(a, b) ((int32)((uint8 *)(b) - (uint8 *)(a)))
#define BUFFER_OFFSET(buffer, offset) ((uint8 *)(buffer) + (offset))
#define STRIDED_POINTER(type, buffer, stride, index) (type *)((uint8 *)(buffer) + (stride) * (index))
#define STRIDED_READ(type, buffer, stride, index) (*(type *)((uint8 *)(buffer) + (stride) * (index)))

#define ARRAY_LENGTH(type, array) (sizeof(array) / sizeof(type))

struct Str {
    int32 length;
    char *string;
};


uint32 Hash(const char *str) {
    uint32 result = 0;
    uint32 len = strlen(str);
    for (int i = 0; i < len; i++) {
        result = result * 33 + str[i];
    }

    return result;
}

#define HasFlag(bitfield, flag) ((bitfield) & (flag))
#define HasFlags(bitfield, flag) ((flag) != 0 && ((bitfield) & (flag)) == (flag))
#define AddFlag(bitfield, flag) ((bitfield) | (flag))
#define ClearFlag(bitfield, flag) ((bitfield) & ~(flag))


#if _WIN32
#define CompareAndSwap(dest, comparand, exchange) InterlockedCompareExchange((LONG volatile *)dest, (LONG)exchange, (LONG)comparand)
#define CompareAndSwap64(dest, comparand, exchange) InterlockedCompareExchange64((LONG64 volatile *)dest, (LONG64)exchange, (LONG64)comparand)
#define AtomicAdd(value, addend) _InterlockedExchangeAdd64((LONGLONG volatile *)value, (LONGLONG)addend)
#define AtomicExchange(ptr, value) InterlockedExchange(ptr, value)
#define AtomicExchange64(ptr, value) InterlockedExchange64((LONG64 *)ptr, (LONG64)value)
#define ThreadYield() SwitchToThread()

#define CycleCount() __rdtsc()
#endif


struct SpinLock {
    uint32 value;
};

inline void AcquireLock(SpinLock *lock) {
  while (true) {
    if (0 == CompareAndSwap(&lock->value, 0, 1)) {
      break;
    }
    // @TODO: maybe yield or pause
  }
}


inline void ReleaseLock(SpinLock *lock) {
  // I think on Linux we want to use __sync_lock_release instead
  AtomicExchange(&lock->value, 0);
}
