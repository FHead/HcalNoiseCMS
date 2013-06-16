#include "ChannelChargeMix.h"

#include "geners/binaryIO.hh"
#include "geners/IOException.hh"

using namespace gs;

bool ChannelChargeMix::operator==(const ChannelChargeMix& r) const
{
    if (channelIndex == UINT_MAX)
        return r.channelIndex == UINT_MAX;

    if (!(chargeResponse    == r.chargeResponse    &&
          Energy            == r.Energy            &&
          RecHitTime        == r.RecHitTime        &&
          FlagWord          == r.FlagWord          &&
          AuxWord           == r.AuxWord           &&
          channelIndex      == r.channelIndex))
        return false;

    for (unsigned i=0; i<nTimeSlices; ++i)
        if (Charge[i] != r.Charge[i])
            return false;

    for (unsigned i=0; i<nTimeSlices; ++i)
        if (addedCharge[i] != r.addedCharge[i])
            return false;    

    return true;
}

bool ChannelChargeMix::write(std::ostream& of) const
{
    write_pod_array(of, Charge, nTimeSlices);
    write_pod_array(of, addedCharge, nTimeSlices);
    write_pod(of, chargeResponse);
    write_pod(of, Energy);
    write_pod(of, RecHitTime);
    write_pod(of, FlagWord);
    write_pod(of, AuxWord);
    write_pod(of, channelIndex);
    return !of.fail();
}

void ChannelChargeMix::restore(const gs::ClassId& id, std::istream& in,
                               ChannelChargeMix* ptr)
{
    static const ClassId myClassId(ClassId::makeId<ChannelChargeMix>());
    myClassId.ensureSameId(id);

    assert(ptr);
    read_pod_array(in, ptr->Charge, nTimeSlices);
    read_pod_array(in, ptr->addedCharge, nTimeSlices);
    read_pod(in, &ptr->chargeResponse);
    read_pod(in, &ptr->Energy);
    read_pod(in, &ptr->RecHitTime);
    read_pod(in, &ptr->FlagWord);
    read_pod(in, &ptr->AuxWord);
    read_pod(in, &ptr->channelIndex);

    if (in.fail()) throw IOReadFailure("In ChannelChargeMix::restore: "
                                       "input stream failure");
}
