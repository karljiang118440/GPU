##############################################################################
#
# Freescale Confidential Proprietary
#
# Copyright (c) 2016 Freescale Semiconductor;
# All Rights Reserved
#
##############################################################################
#
# THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################

SDK_ROOT := $(call path_relative_to,$(CURDIR),$(CURR_SDK_ROOT))

VIVANTE_SDK_DIR :=/media/jcq/study/GPU/source/GPU_userspace_binaries_and_VTK_tools_6.2.4/Vivante_userspace_libraries_and_demos/gpu-viv-bin-s32v234-6.2.4.p2-hardfp/usr


##############################################################################
# ARM_APP
##############################################################################

ARM_APP = gpu_hello

ARM_APP_SRCS =                                                               \
    tutorial.cpp                                                                 \

ARM_INCS =                                                                   \
    -I.																		 \
	-I$(VIVANTE_SDK_DIR)/include 											 \


ARM_LDOPTS +=                                                                \
	-L$(VIVANTE_SDK_DIR)/lib 												 \
	-Wl,-rpath-link=$(VIVANTE_SDK_DIR)/lib                              	 \
	-lEGL																	 \
	-lGLESv2                                                                 \
	-lVDK

ARM_DEFS += -DLINUX -DEGL_API_FB -DGPU_TYPE_VIV -DGL_GLEXT_PROTOTYPES
