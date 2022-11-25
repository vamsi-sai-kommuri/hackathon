#include "test_generated.h" // This was generated by `flatc`.
#include <iostream>
 
using namespace NetworkModels::CAN1; // Specified in the schema.

int main(int /*argc*/, const char * /*argv*/[]) {
// Create a `FlatBufferBuilder`, which will be used to create our
// monsters' FlatBuffers.
flatbuffers::FlatBufferBuilder builder(1024);

auto mFrame = CreateMetaFrame(builder,CanFDIndicator_canFD);

std::vector<flatbuffers::Offset<MetaFrame>> MetaFrameVector;
MetaFrameVector.push_back(mFrame);

auto metaFrame = builder.CreateVector(MetaFrameVector);
auto fileR = CreateRegisterFile(builder,metaFrame);

builder.Finish(fileR);

uint8_t *buf = builder.GetBufferPointer();
int size = builder.GetSize(); 

std::cout << "size of buffer : " << size << std::endl;  

auto frame = GetRegisterFile(buf);

auto framedata = frame->buffer();

auto name = framedata->Get(0)->canFD_enabled();


std::cout << "CanFd status: " << name << std::endl;  

}

