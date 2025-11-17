cc_library(
    name = "gloo",
    srcs = [],
    hdrs = glob(["gloo/**/*.h"]),
    includes = ["include"], # 指定头文件根目录
    visibility = ["//visibility:public"],
)