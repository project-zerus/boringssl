include_directories(. .. ../../include)

add_library(
  hmac

  OBJECT

  hmac.c
)


add_executable(
  hmac_test

  hmac_test.cc
)

target_link_libraries(hmac_test crypto)
