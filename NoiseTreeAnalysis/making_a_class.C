{
    TFile f("NoiseTree_107.root", "READ");
    TDirectoryFile* df = (TDirectoryFile*)f.Get("ExportTree");
    TTree* t = (TTree*)df->Get("HcalNoiseTree");
    t->MakeClass("NoiseTreeData");
}
