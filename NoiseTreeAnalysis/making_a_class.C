{
    TFile f("NoiseTree_210.root", "READ");
    TDirectoryFile* df = (TDirectoryFile*)f.Get("ExportTree");
    TTree* t = (TTree*)df->Get("HcalNoiseTree");
    t->MakeClass("NoiseTreeData");
}
