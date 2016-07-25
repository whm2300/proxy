#ifndef DEFINE_H_
#define DEFINE_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);             \
    TypeName& operator=(const TypeName&)

#define FORCEINLINE  __inline__ __attribute__((always_inline))

#define NOT_USE(x) (void)x

#endif  //DEFINE_H_
