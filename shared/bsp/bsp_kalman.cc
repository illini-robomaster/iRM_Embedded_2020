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

Kalman::Kalman(uint8_t dims, Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B)
     : F_(F), H_(H), B_(B){
 x_.Eigen::resize(dims);
 P_.Eigen::resize(dims, dims);

 x_.Eigen::setZero(dims);
 P_.Eigen::setIdentity(dims, dims); 
 Q_.Eigen::setIdentity(dims, dims);
 R_.Eigen::setIdentity(dims, dims);
}

void Kalman::Predict(Eigen::VectorXf u) {
 x_ = F_ * x_ + B_ * u;
 P_ = F_ * P_ * F_.Eigen::transpose() + Q_;
}

void Kalman::Update(Eigen::VectorXf y) {
 Eigen::Matrix<float, dims, dims> S = H_ * P_ * H_.Eigen::transpose() + R_;
 Eigen::Matrix<float, dims, dims> K = P_ * H_.Eigen::transpose() * S.Eigen::inverse();
 x_ = x_ + K * (y - H_ * x);
 P_ = (Eigen::Matrix<float, dims, dims>::Identity() - K * H_) * P_;
}

} // namespace bsp ends
