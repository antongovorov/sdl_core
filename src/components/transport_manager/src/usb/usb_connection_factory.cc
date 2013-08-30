/**
 * \file usb_connection_factory.cc
 * \brief
 *
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <libusb-1.0/libusb.h>

#include "transport_manager/usb/usb_connection_factory.h"
#include "transport_manager/usb/usb_device.h"
#include "transport_manager/usb/usb_connection.h"
#include "transport_manager/device_adapter/device_adapter_impl.h"

namespace transport_manager {
namespace device_adapter {

UsbConnectionFactory::UsbConnectionFactory(DeviceAdapterController* controller)
    : controller_(controller) {
}

DeviceAdapter::Error UsbConnectionFactory::init() {
  return DeviceAdapter::OK;
}

DeviceAdapter::Error UsbConnectionFactory::createConnection(
    const DeviceUID& device_uid, const ApplicationHandle& app_handle) {
  DeviceSptr device = controller_->findDevice(device_uid);
  if (!device.valid()) {
    LOG4CXX_ERROR(logger_, "device " << device_uid << " not found");
    return DeviceAdapter::BAD_PARAM;
  }

  UsbDevice* usb_device = static_cast<UsbDevice*>(device.get());
  UsbConnection* usb_connection = new UsbConnection(device_uid, app_handle,
                                                    controller_,
                                                    usb_device->usb_device());
  ConnectionSptr connection(usb_connection);

  if (!usb_connection->Init()) {
    return DeviceAdapter::FAIL;
  }

  controller_->connectionCreated(connection, device_uid, app_handle);
  controller_->connectDone(device_uid, app_handle);

  return DeviceAdapter::OK;
}

void UsbConnectionFactory::terminate() {
}

bool UsbConnectionFactory::isInitialised() const {
  return true;
}

UsbConnectionFactory::~UsbConnectionFactory() {
}

}  // namespace device_adapter
}  // namespace transport_manager
