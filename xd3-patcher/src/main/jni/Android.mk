# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := xd3-patcher
LOCAL_SRC_FILES := patcher.c

# compiled library for android6.0+
LOCAL_LDFLAGS += -pie -fPIE -shared
LOCAL_CFLAGS += -pie -fPIE -fvisibility=hidden

include $(BUILD_SHARED_LIBRARY)
