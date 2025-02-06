#Minimum Cmake Version (My version: 3.22.1)
cmake_minimum_required(VERSION 3.10)

#Definire nume proiect + limbaj folosit:
project(TrainApp C CXX) #Comanda project este folosita pentru a defini in ce proiect folosim Cmake si ce limbaje, optional se poate oferi o versiune a proiectului;

#Ptr limbajele alese trebuie setate niste standarde:
set(CMAKE_C_STANDARD 99) # Folosim C99 ca standard
set(CMAKE_CXX_STANDARD 17) #Folosim C++17 ca standard
set(CMAKE_C_STANDARD_REQUIRED true) #Compiler trubuie sa poata rula acest standard ptr C
set(CMAKE_CXX_STANDARD_REQUIRED true) #Compiler trubuie sa poata rula acest standard ptr C++

#Pentru LibXML2:
find_package(LibXml2 REQUIRED)

#Headers ptr LibXML2:
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories("/home/alex/include")

#Definim fisierele sursa:
set(SURS_SERVER
        server.cpp
        Classes/XMLWorkAround.cpp
        Classes/XMLWorkAround.h
        Classes/RegLog.cpp
        Classes/RegLog.h
        Classes/Helpers.cpp
        Classes/Helpers.h
        Classes/JSONMethods.cpp
        Classes/JSONMethods.h
)
#Definim o variabila SURS_SERVER ce va lua path-ul pentru server.cpp

#Definim fisierele sursa:
set(SURS_CLIENT
        client.cpp
        Classes/XMLWorkAround.cpp
        Classes/XMLWorkAround.h
        Classes/RegLog.cpp
        Classes/RegLog.h
        Classes/Helpers.cpp
        Classes/Helpers.h
        Classes/JSONMethods.cpp
        Classes/JSONMethods.h
)
#Definim o variabila SURS_CLIENT ce va lua path-ul pentru client.cpp

#Cream un executabil:
add_executable(ServerPart ${SURS_SERVER})
add_executable(ClientPart ${SURS_CLIENT})

#Legam LibXML2 de cele 2 executabile:
target_link_libraries(ServerPart ${LIBXML2_LIBRARIES} pthread)
target_link_libraries(ClientPart ${LIBXML2_LIBRARIES} pthread)
