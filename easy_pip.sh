cp build/anaroutePy.cpython-37m-x86_64-linux-gnu.so ~/.local/lib/python3.7/site-packages/


# if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
#   if(EXISTS "$ENV{DESTDIR}/usr/local/bin/anaroute" AND
#      NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/anaroute")
#     file(RPATH_CHECK
#          FILE "$ENV{DESTDIR}/usr/local/bin/anaroute"
#          RPATH "")
#   endif()
#   list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
#    "/usr/local/bin/anaroute")
#   if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
#     message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
#   endif()
#   if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
#     message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
#   endif()
# file(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/MAGICAL_modified/anaroute/bin/anaroute")
#   if(EXISTS "$ENV{DESTDIR}/usr/local/bin/anaroute" AND
#      NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/bin/anaroute")
#     if(CMAKE_INSTALL_DO_STRIP)
#       execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/anaroute")
#     endif()
#   endif()
# endif()
