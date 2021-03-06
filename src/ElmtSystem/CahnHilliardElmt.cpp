//****************************************************************
//* This file is part of the AsFem framework
//* A Simple Finite Element Method program (AsFem)
//* All rights reserved, Yang Bai @ CopyRight 2021
//* https://github.com/yangbai90/AsFem.git
//* Licensed under GNU GPLv3, please see LICENSE for details
//* https://www.gnu.org/licenses/gpl-3.0.en.html
//****************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++ Author : Yang Bai
//+++ Date   : 2020.12.30
//+++ Purpose: implement the residual and jacobian for general
//+++          Cahn-Hilliard equation
//+++          dc/dt=div(M*grad(mu))
//+++             mu=d2f/dc-Kappa*Delta(c)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "ElmtSystem/BulkElmtSystem.h"

void BulkElmtSystem::CahnHilliardElmt(const FECalcType &calctype,
                                      const int &nDim, const int &nNodes,const int &nDofs,
                                      const double &t,const double &dt, const double (&ctan)[2],
                                      const Vector3d &gpCoords,
                                      const vector<double> &gpU, const vector<double> &gpV,
                                      const vector<Vector3d> &gpGradU, const vector<Vector3d> &gpGradV,
                                      const double &test, const double &trial, const Vector3d &grad_test,
                                      const Vector3d &grad_trial, const ScalarMateType &ScalarMaterials,
                                      const VectorMateType &VectorMaterials, const Rank2MateType &Rank2Materials,
                                      const Rank4MateType &Rank4Materials, vector<double> &gpHist,
                                      vector<double> &gpHistOld, vector<double> &gpProj, MatrixXd &localK,
                                      VectorXd &localR) {
    //*******************************************************
    //*** to get rid of the warning for unused variables  ***
    //*** for normal users, you dont need to do this      ***
    //*******************************************************
    if(nDim||nNodes||nDofs||t||dt||ctan[0]||gpCoords(1)||gpU.size()||gpV.size()||
       gpGradU.size()||gpGradV.size()||test||trial||grad_test(1)||grad_trial(1)||
       ScalarMaterials.size()||VectorMaterials.size()||Rank2Materials.size()||Rank4Materials.size()||
       gpHist.size()||gpHistOld.size()||gpProj.size()){}

    // Dofs: C -->1
    //       Mu-->2
    switch (calctype){
        case FECalcType::ComputeResidual:
            // For R_c
            localR(1)=gpV[1]*test
                       +ScalarMaterials.at("M")*(gpGradU[2]*grad_test);
            // For R_mu
            localR(2)=gpU[2]*test
                     -ScalarMaterials.at("dFdc")*test
                     -ScalarMaterials.at("Kappa")*(gpGradU[1]*grad_test);
            break;
        case FECalcType::ComputeJacobian:
            // K_c,c
            localK(1,1)=trial*test*ctan[1]
                           +ScalarMaterials.at("dMdc")*trial*(gpGradU[2]*grad_test)*ctan[0];
            // K_c,mu
            localK(1,2)=ScalarMaterials.at("M")*grad_trial*grad_test;
            // K_mu,c
            localK(2,1)=-ScalarMaterials.at("d2Fdc2")*trial*test*ctan[0]
                       -ScalarMaterials.at("Kappa")*grad_trial*grad_test*ctan[0];
            // K_mu,mu
            localK(2,2)=trial*test*ctan[0];
            break;
        case FECalcType::InitHistoryVariable:
            gpHist[0]=0.0;
            break;
        case FECalcType::UpdateHistoryVariable:
            gpHistOld=gpHist;
            break;
        case FECalcType::Projection:
            gpProj[0]=gpU[1];
            gpProj[1]=gpGradU[1](1);
            gpProj[2]=gpGradU[1](2);
            gpProj[3]=gpGradU[1](3);
            break;
        default:
            MessagePrinter::PrintErrorTxt("unsupported FEM calculation type in Poisson element");
            MessagePrinter::AsFem_Exit();
            break;
    }
}