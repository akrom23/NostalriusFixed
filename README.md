
Nostalrius with some changes to allow compile of extractors and the "frankensteins world DB"


Steps to create (on linux)
--------------------------

Pre-requisites
--------------

You must have the minimum required development packages/libraries in order to build stock mangos.


Library installations:
----------------------

tbb (ubuntu: apt-get install libtbb-dev)


Build core:
-----------

mkdir build

cd build

cmake .. -DCMAKE_INSTALL_PREFIX=<install location>

make -j <cores>

make install


Build mpqlib
------------

cd dep/libmpq

./autogen.sh

./configure

make -j <cores>


Build extractors
----------------

cd contrib/extractor

mkdir build

cd build

make -j <cores>


(move the ad file from build folder to where you keep client files)


cd ../..

cd contrib/vmap_extractor

mkdir build

cd build

make -j <cores>


(move the vmapextractor from the build/vmapextractor to where you keep client files)


cd ../..

cd contrib/vmap_assembler

mkdir build

cd build

make -j <cores>


(move the vmap_assembler from the build folder to where you keep client files)

cd ../../

cd contrib/mmap

mkdir build

make -j <cores>


(move the MoveMapGen from the build folder to where you keep client files)


Build vmap extractor from stock old mangos
------------------------------------------

Change to a new folder away from this repo

Enter

git clone https://bitbucket.org/mangoszero/server.git old-mangos

cd old-mangos

git reset --hard 389c8c72997b36008647c47d0c6e6996ce19218f


This will give you a mangos repo at the point required. Use the normal method to compile mangos + tools. It might fail. But you should have vmap-extractor in the build/bin folder. Copy this with a new name vmap-extractor-org or similar.

Build data:
-----------
In the normal way, run ad to extract maps, then the original mangos vmap-extractor, then the nostalrius vmap-extractor, then the vmap-assembler, and finally MoveMapGen. The folders dbc, maps, vmaps and mmaps should be copied/moved to the server binary location.

Database:
---------

Create 4 databases, name them how you like. One for auth, one for characters, one for world and one for logs.

in the sql folder, use the sql scripts logon.sql on the auth database to create the structure, characters.sql on character DB to create structure, logs.sql on the logs DB to create structure

Then apply world_all.sql to the world DB. This should create the structure and data for the "frankensteins" DB, made up of the world.sql in sandbox, and applying all SQL files found here.

At this point it's usual Mangos fayre, add realm to realmlist table, start realmd, then mangosd, create account, login.
