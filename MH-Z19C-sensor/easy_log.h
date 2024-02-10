
#ifndef EASY_LOG_H
#define EASY_LOG_H

#ifdef __cplusplus
extern "C" {
#endif


size_t snprintfcat(char* buf,size_t bufSize,char const* fmt,...);

void   MyTrace(const char *fileName, int line, const char *msg, ...);

#define LOG(msg) \
 MyTrace(__FILE__, __LINE__, msg, NULL)

#define ERR(msg, ...) \
 MyTrace(__FILE__, __LINE__, msg, __VA_ARGS__)

#define ERR_L(msg) \
 MyTrace(__FILE__, __LINE__, msg, NULL)

#define TRACE(msg, ...) \
 MyTrace(__FILE__, __LINE__, msg, __VA_ARGS__)

#define DEBUG(msg, ...) \
 MyTrace(__FILE__, __LINE__, msg, __VA_ARGS__)


#ifdef __cplusplus
}
#endif
#endif /* EASY_LOG_H */
