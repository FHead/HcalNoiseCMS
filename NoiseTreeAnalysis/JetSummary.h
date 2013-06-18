#ifndef JetSummary_h_
#define JetSummary_h_

struct JetSummary
{
    inline JetSummary()
        : NJetsMade(0),
          NJetsUsed(0),
          JetCount20(0),
          JetCount30(0),
          JetCount50(0),
          JetCount100(0),
          LeadingJetEta(0.0),
          LeadingJetPhi(0.0),
          LeadingJetPt(-1.0),
          FollowingJetEta(0.0),
          FollowingJetPhi(0.0),
          FollowingJetPt(-1.0),
          EtSum(-1.0),
          EtFractionUsed(-0.1)
    {}

    unsigned NJetsMade;
    unsigned NJetsUsed;
    unsigned JetCount20;
    unsigned JetCount30;
    unsigned JetCount50;
    unsigned JetCount100;
    double LeadingJetEta;
    double LeadingJetPhi;
    double LeadingJetPt;
    double FollowingJetEta;
    double FollowingJetPhi;
    double FollowingJetPt;
    double EtSum;
    double EtFractionUsed;
};

#endif // JetSummary_h_
