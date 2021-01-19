/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2021 RoboMaster.                                          *
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

#pragma once

#include <Eigen/Dense>

namespace bsp{

class Kalman {
 public:
 
 Kalman(uint8_t dims, Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B);
 Eigen::VectorXf GetEstimate(void);
 void Predict(Eigen::VectorXf input);
 void Update(Eigen::VectorXf observation);
 
 private:
 uint8_t dims_;
 Eigen::VectorXf x_;    // estimation in wiki Kalman filter formula
 Eigen::MatrixXf P_;    // covariance in wiki Kalman filter formula
 
 Eigen::MatrixXf F_;    // transfer matrix: x_k -> x_k+1
 Eigen::MatrixXf H_;    // transfer matrix: observation y -> x_k+1
 Eigen::MatrixXf B_;    // transfer matrix: input u -> x

 Eigen::MatrixXf Q_;    // noise covariance
 Eigen::MatrixXf R_;    // noise covariance 
};



} // namespace bsp ends

