#include "rmw_fastrtps_cpp/MessageTypeSupport.h"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"

#include <fastcdr/FastBuffer.h>
#include <fastcdr/Cdr.h>

#include <cassert>
#include <memory>

using namespace rmw_fastrtps_cpp;

MessageTypeSupport::MessageTypeSupport(const rosidl_typesupport_introspection_cpp::MessageMembers *members)
{
    assert(members);
    members_ = members;

    std::string name = std::string(members->package_name_) + "::dds_::" + members->message_name_ + "_";
    setName(strdup(name.c_str()));
    m_typeSize = calculateMaxSerializedSize(members, 0);
    m_isGetKeyDefined = false;
}

bool MessageTypeSupport::serialize(void *data, SerializedPayload_t *payload)
{
    assert(data);
    assert(payload);

    Buffer *buffer = static_cast<Buffer*>(data);
    payload->length = buffer->length;
    memcpy(payload->data, buffer->pointer, buffer->length);
    return true;
}

bool MessageTypeSupport::deserialize(SerializedPayload_t *payload, void *data)
{
    assert(data);
    assert(payload);

    Buffer *buffer = static_cast<Buffer*>(data);
    buffer->length = payload->length;
    memcpy(buffer->pointer, payload->data, payload->length);
    return true;
}

void* MessageTypeSupport::createData()
{
    Buffer *buffer = static_cast<Buffer*>(malloc(sizeof(Buffer) + m_typeSize));

    if(buffer)
    {
        buffer->length = 0;
        buffer->pointer = (char*)(buffer + 1);
    }

    return buffer;
}

bool MessageTypeSupport::serializeROSmessage(const void *ros_message, Buffer *buffer)
{
    assert(buffer);
    assert(ros_message);

    eprosima::fastcdr::FastBuffer fastbuffer(buffer->pointer, m_typeSize);
    eprosima::fastcdr::Cdr ser(fastbuffer);

    TypeSupport::serializeROSmessage(ser, members_, ros_message);

    buffer->length = (uint32_t)ser.getSerializedDataLength();
    return true;
}

bool MessageTypeSupport::deserializeROSmessage(const Buffer *buffer, void *ros_message)
{
    assert(buffer);
    assert(ros_message);

    eprosima::fastcdr::FastBuffer fastbuffer(buffer->pointer, buffer->length);
    eprosima::fastcdr::Cdr deser(fastbuffer);

    TypeSupport::deserializeROSmessage(deser, members_, ros_message, false);

    return true;
}
