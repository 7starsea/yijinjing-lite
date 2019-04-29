# Copyright [2017] [taurus.ai]
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_executable(writer writer.cpp stat.cpp)
TARGET_LINK_LIBRARIES(writer paged journal)

add_executable(reader reader.cpp stat.cpp)
TARGET_LINK_LIBRARIES(reader paged journal)

add_executable(yjj_page_service page_service.cpp)
TARGET_LINK_LIBRARIES(yjj_page_service paged journal)

INSTALL(TARGETS writer reader yjj_page_service  DESTINATION "bin/")
