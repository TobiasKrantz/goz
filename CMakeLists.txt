project(goz)

file(GLOB SOURCES *.cpp *.h)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
