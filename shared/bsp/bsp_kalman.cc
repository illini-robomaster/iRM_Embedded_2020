/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2020 RoboMaster.                                          *
 *  Illini RoboMaster @ University of Illinois at Urbana-Champaign          *
 *                                                                          *
 *  This program is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                          *
 ****************************************************************************/
 
#include "bsp_kalman.h"

namespace bsp {

Kalman::Kalman(Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B)
     : F_(F), H_(H), B_(B){
 dims_ = F.rows();
 x_.setZero(dims_);
 P_.setIdentity(dims_, dims_); 
 Q_.setIdentity(dims_, dims_);
 R_.setIdentity(dims_, dims_);
}

Kalman::Kalman(Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B, Eigen::MatrixXf Q, Eigen::MatrixXf R)
     : F_(F), H_(H), B_(B), Q_(Q), R_(R){
 dims_ = F.rows();
 x_.resize(dims_);
 P_.resize(dims_, dims_);

 x_.setZero(dims_);
 P_.setIdentity(dims_, dims_);
}

void Kalman::SetInit(Eigen::VectorXf x, Eigen::MatrixXf P) {
 x_ = x;
 P_ = P;
}

Eigen::VectorXf Kalman::GetEstimate(Eigen::VectorXf u) {
 x_ = F_ * x_ + B_ * u;
 return x_;
}

Eigen::VectorXf Kalman::GetEstimate(void) {
 x_ = F_ * x_;
 return x_;
}

void Kalman::Predict(Eigen::VectorXf u) {
 x_ = F_ * x_ + B_ * u;
 P_ = F_ * P_ * F_.transpose() + Q_;
}


void Kalman::Predict(void) {
 x_ = F_ * x_;
 P_ = F_ * P_ * F_.transpose() + Q_;
}

void Kalman::Update(Eigen::VectorXf y) {
 Eigen::MatrixXf S = H_ * P_ * H_.transpose() + R_;
 Eigen::MatrixXf K = P_ * H_.transpose() * S.inverse();
 x_ = x_ + K * (y - H_ * x_);
 P_ = (Eigen::MatrixXf::Identity(dims_, dims_) - K * H_) * P_;
}

Eigen::VectorXf Kalman::GetState(void) {
 return x_;
}

} // namespace bsp ends
