/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote prducts derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ROS_SERVICE_CLIENT_H_
#define _ROS_SERVICE_CLIENT_H_

#include "rosserial_msgs/msg/topic_info.hpp"

#include "ros/publisher.h"
#include "ros/subscriber.h"

using namespace rosserial_msgs::msg;

namespace ros
{

template<typename ServiceT>
class ServiceClient : public Subscriber_
{
public:

  ServiceClient(const char* topic_name) :
    pub(topic_name, &req, TopicInfo::ID_SERVICE_CLIENT + TopicInfo::ID_PUBLISHER)
  {
    this->topic_ = topic_name;
    this->waiting = true;
  }

  bool call(const typename ServiceT::Request & request, typename ServiceT::Response & response)
  {
    if (!pub.nh_->connected())
        return false;

    ret = &response;
    waiting = true;
    pub.publish(&request);
    while (waiting && pub.nh_->connected()) {
      if (pub.nh_->spinOnce() < 0) {
        return false;
      }
    }

    return !waiting && pub.nh_->connected();
  }

  // these refer to the subscriber
  virtual void callback(ucdrBuffer *data) override
  {
    ret->cdr_deserialize(data);
    waiting = false;
  }

  virtual const char * getMsgType() override
  {
    return this->resp.getType();
  }
  
  virtual const char * getMsgMD5() override
  {
    return this->resp.getMD5();
  }

  virtual int getEndpointType() override
  {
    return rosserial_msgs::msg::TopicInfo::ID_SERVICE_CLIENT + rosserial_msgs::msg::TopicInfo::ID_SUBSCRIBER;
  }

  typename ServiceT::Request req;
  typename ServiceT::Response resp;
  typename ServiceT::Response * ret;
  bool waiting;
  Publisher pub;
};

}

#endif
