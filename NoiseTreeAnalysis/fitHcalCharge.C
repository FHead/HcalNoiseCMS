#include <cassert>
#include <cstring>
#include <cmath>
#include <cfloat>

#include "fitHcalCharge.h"
#include "npstat/nm/lapack.h"


unsigned minSampleSizeForHcalChargeFit(const unsigned minTSToFit,
                                       const unsigned maxTS,
                                       const unsigned fitOrder)
{
    // Check that the TS specification makes sense
    assert(minTSToFit < maxTS);
    assert(maxTS <= 10U);

    const unsigned numTSToFit = maxTS - minTSToFit;
    unsigned minSize = 0;

    switch (fitOrder)
    {
    case 1U:
        minSize = numTSToFit + 2U;
        break;

    case 2U:
        minSize = (numTSToFit+1U)*(numTSToFit+2U)/2U + 1U;
        break;

    default:
        assert(!"Unsupported fit order");
        break;
    }

    return minSize;
}


double fitHcalCharge(const std::vector<std::array<float,10> >& predictor,
                     const unsigned minTSToFit, const unsigned maxTS,
                     const std::vector<float>& response,
                     const std::vector<float>& uncertainty,
                     double fittedCoeffs[10], double* constantTerm)
{
    static std::vector<double> mem;
    static std::vector<double> workBuf;
    static std::vector<int> intBuf;
    static unsigned maxNPoints = 0U;
    static unsigned maxTSFitted = 0U;

    // Do we have uncertainties?
    const bool useUncert = !uncertainty.empty();

    // Check input consistency
    const unsigned nPoints = predictor.size();
    assert(nPoints == response.size());
    if (useUncert)
        assert(nPoints == uncertainty.size());

    // Require a minimum number of points for this to be a least squares problem
    assert(nPoints >= minSampleSizeForHcalChargeFit(minTSToFit, maxTS, 1U));

    // Allocate memory for data storage
    const unsigned numTSToFit = maxTS - minTSToFit;
    if (nPoints > maxNPoints || numTSToFit > maxTSFitted)
        if (mem.size() < nPoints*(numTSToFit + 2U))
            mem.resize(nPoints*(numTSToFit + 2U));

    // Copy the data into local buffers in the format
    // appropriate for subsequent DGELSD call
    double* A = &mem[0];
    double* b = A + (numTSToFit + 1U)*nPoints;
    double w = 1.0;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        if (useUncert)
        {
            const double u = uncertainty[ipt];
            assert(u > 0.0);
            w = 1.0/u/u;
        }
        const float* timeSlices = &predictor[ipt][minTSToFit];
        for (unsigned i=0; i<numTSToFit; ++i)
            A[ipt + i*nPoints] = timeSlices[i]*w;
        A[ipt + numTSToFit*nPoints] = w;
        b[ipt] = response[ipt]*w;
    }

    // Allocate memory for DGELSD
    double singularValues[11];
    if (nPoints > maxNPoints || numTSToFit > maxTSFitted)
    {
        maxNPoints = nPoints;
        maxTSFitted = numTSToFit;

        int M = nPoints;
        int N = numTSToFit + 1U;
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
        int N = numTSToFit + 1U;
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

    // Fill out the fit results
    memset(fittedCoeffs, 0, 10*sizeof(double));
    for (unsigned i=0; i<numTSToFit; ++i)
        fittedCoeffs[i + minTSToFit] = b[i];

    const double c = b[numTSToFit];
    if (constantTerm)
        *constantTerm = c;

    // Calculate the RMS
    long double sumsq = 0.0L;
    double sigma = 1.0;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        if (useUncert)
            sigma = uncertainty[ipt];
        double fitted = c;
        const float* in = &predictor[ipt][0];
        for (unsigned i=minTSToFit; i<maxTS; ++i)
            fitted += fittedCoeffs[i]*in[i];
        const double delta = (fitted - response[ipt])/sigma;
        sumsq += delta*delta;
    }
    return sqrt(static_cast<double>(sumsq/nPoints));
}


double fitHcalChargeSq(const std::vector<std::array<float,10> >& predictor,
                       const unsigned minTSToFit, const unsigned maxTS,
                       const std::vector<float>& response,
                       const std::vector<float>& uncertainty,
                       npstat::Matrix<double>* fittedSqCoeffs,
                       double fittedCoeffs[10], double* constantTerm)
{
    static std::vector<double> mem;
    static std::vector<double> workBuf;
    static std::vector<int> intBuf;
    static unsigned maxNPoints = 0U;
    static unsigned maxTSFitted = 0U;
    static const npstat::Matrix<double> zeroMat(10U, 10U, 0);

    // Initialize the result matrix
    assert(fittedSqCoeffs);
    // fittedSqCoeffs->reset();
    *fittedSqCoeffs = zeroMat;

    // Do we have uncertainties?
    const bool useUncert = !uncertainty.empty();

    // Check input consistency
    const unsigned nPoints = predictor.size();
    assert(nPoints == response.size());
    if (useUncert)
        assert(nPoints == uncertainty.size());

    // Require a minimum number of points for this to be a least squares problem
    const unsigned numTerms = 
        minSampleSizeForHcalChargeFit(minTSToFit, maxTS, 2U) - 1U;
    assert(nPoints > numTerms);

    // Allocate memory for data storage
    const unsigned numTSToFit = maxTS - minTSToFit;
    if (nPoints > maxNPoints || numTSToFit > maxTSFitted)
        if (mem.size() < nPoints*(numTerms + 1U))
            mem.resize(nPoints*(numTerms + 1U));

    // Copy the data into local buffers in the format
    // appropriate for subsequent DGELSD call
    double* A = &mem[0];
    double* b = A + numTerms*nPoints;
    double w = 1.0;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        if (useUncert)
        {
            const double u = uncertainty[ipt];
            assert(u > 0.0);
            w = 1.0/u/u;
        }
        const float* timeSlices = &predictor[ipt][minTSToFit];
        for (unsigned i=0; i<numTerms; ++i)
        {
            double poly = DBL_MAX;
            if (i == 0U)
                poly = 1.0;
            else if (i <= numTSToFit)
                poly = timeSlices[i - 1U];
            else
            {
                const unsigned qnum = i - numTSToFit - 1U;
                for (unsigned k=1; k<=numTSToFit; ++k)
                    if (qnum < k*(k+1U)/2U)
                    {
                        const unsigned j = k - 1U;
                        poly = timeSlices[j]*timeSlices[qnum - k*j/2U];
                        break;
                    }
            }
            assert(poly < 0.99*DBL_MAX);
            A[ipt + i*nPoints] = poly*w;
        }
        b[ipt] = response[ipt]*w;
    }

    // Allocate memory for DGELSD
    double singularValues[66];
    if (nPoints > maxNPoints || numTSToFit > maxTSFitted)
    {
        maxNPoints = nPoints;
        maxTSFitted = numTSToFit;

        int M = nPoints;
        int N = numTerms;
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
        int N = numTerms;
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

    // Fill out the fit results
    const double c = b[0];
    if (constantTerm)
        *constantTerm = c;

    memset(fittedCoeffs, 0, 10*sizeof(double));
    unsigned cnt = numTSToFit + 1;
    for (unsigned i=0; i<numTSToFit; ++i)
    {
        fittedCoeffs[i + minTSToFit] = b[i + 1];
        for (unsigned j=0; j<=i; ++j, ++cnt)
        {
            if (i == j)
                (*fittedSqCoeffs)[i + minTSToFit][j + minTSToFit] = b[cnt];
            else
            {
                (*fittedSqCoeffs)[i + minTSToFit][j + minTSToFit] = b[cnt]/2.0;
                (*fittedSqCoeffs)[j + minTSToFit][i + minTSToFit] = b[cnt]/2.0;
            }
        }
    }

    // Calculate the RMS
    long double sumsq = 0.0L;
    double sigma = 1.0;
    for (unsigned ipt=0; ipt<nPoints; ++ipt)
    {
        if (useUncert)
            sigma = uncertainty[ipt];
        double fitted = c;
        const float* in = &predictor[ipt][0];
        for (unsigned i=minTSToFit; i<maxTS; ++i)
            fitted += fittedCoeffs[i]*in[i];
        fitted += fittedSqCoeffs->bilinear(in, 10U);
        const double delta = (fitted - response[ipt])/sigma;
        sumsq += delta*delta;
    }
    return sqrt(static_cast<double>(sumsq/nPoints));
}
