IF(HAVE_SDL)
  ADD_EXECUTABLE(simple
    main.cpp
    simple_example.cpp
  )

  TARGET_LINK_LIBRARIES(simple PRIVATE libvisual SDL::SDL)
ENDIF()
