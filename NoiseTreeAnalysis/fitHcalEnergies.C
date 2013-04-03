#include <cassert>
#include <cmath>

#include "fitHcalEnergies.h"
#include "npstat/nm/lapack.h"

double fitHcalEnergies(const std::vector<std::pair<float, float> >& timeSlices,
                       const std::vector<float>& energies,
                       double fittedCoeffs[3])
{
    static std::vector<double> mem;
    static std::vector<double> workBuf;
    static std::vector<int> intBuf;
    static unsigned maxNPoints = 0U;

    double singularValues[3];

    // Need at least 4 points for this to be a least squares problem
    const unsigned nPoints = timeSlices.size();
    assert(nPoints == energies.size());
    assert(nPoints > 3);

    // Allocate memory for data storage
    if (nPoints > maxNPoints)
        mem.resize(nPoints*4);

    // Copy the data into local buffers in the format
    // appropriate for subsequent DGELSD call
    double* A = &mem[0];
    double* b = A + 3*nPoints;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        A[ipt + 0*nPoints] = timeSlices[ipt].first;
        A[ipt + 1*nPoints] = timeSlices[ipt].second;
        A[ipt + 2*nPoints] = 1.0;
        b[ipt] = energies[ipt];
    }

    // Allocate memory for DGELSD
    if (nPoints > maxNPoints)
    {
        maxNPoints = nPoints;

        int M = nPoints;
        int N = 3;
        int NRHS = 1;
        int LDA = nPoints;
        int LDB = nPoints;
        double RCOND = 1.0e-12;
        int RANK = 0;
        int INFO = 1;
        double wtmp = 0.0;
        int itmp;
        int LWORK = -1;

        dgelsd_(&M, &N, &NRHS, A,
                &LDA, b, &LDB, singularValues,
                &RCOND, &RANK, &wtmp,
                &LWORK, &itmp, &INFO);
        assert(INFO == 0);

        if (itmp > static_cast<int>(intBuf.size()))
            intBuf.resize(itmp);
        const unsigned needDoub = static_cast<unsigned>(wtmp);
        if (needDoub > workBuf.size())
            workBuf.resize(needDoub);
    }

    // Call the LAPACK linear least squares solver DGELSD
    {
        int M = nPoints;
        int N = 3;
        int NRHS = 1;
        int LDA = nPoints;
        int LDB = nPoints;
        double RCOND = 1.0e-12;
        int RANK = 0;
        int INFO = 1;
        int LWORK = workBuf.size();

        dgelsd_(&M, &N, &NRHS, A,
                &LDA, b, &LDB, singularValues,
                &RCOND, &RANK, &workBuf[0],
                &LWORK, &intBuf[0], &INFO);
        assert(INFO == 0);
    }

    fittedCoeffs[0] = b[0];
    fittedCoeffs[1] = b[1];
    fittedCoeffs[2] = b[2];

    long double sumsq = 0.0L;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        const double fitted = fittedCoeffs[0]*timeSlices[ipt].first +
                              fittedCoeffs[1]*timeSlices[ipt].second +
                              fittedCoeffs[2];
        const double delta = fitted - energies[ipt];
        sumsq += delta*delta;
    }
    return sqrt(static_cast<double>(sumsq/nPoints));
}
