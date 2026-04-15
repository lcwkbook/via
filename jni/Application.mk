APP_ABI := arm64-v8a
APP_PLATFORM := android-25
APP_STL := c++_static
APP_OPTIM := release


# Application.mk 中添加
APP_STL := c++_static  # 替换原来的c++_shared

APP_ABI := arm64-v8a    # 只编译arm64-v8a架构，按需添加其他架构如armeabi-v7a
APP_PLATFORM := android-21  # 最低支持的Android版本，建议≥21
