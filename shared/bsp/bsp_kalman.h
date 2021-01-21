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

/**
 * All the symbols stand the same meaning as the wikipedia page for Kalman filter
 */
class Kalman {
 public:
 /**
  * @brief	constructor for kalman filter with n dimensions and uncorrelated noises, both covariance matrices for the noise are identity matrices.
  *
  *
  * @param F	transfer matrix from state(x_k) to the state in the next time frame(x_k+1)
  * @param H	transfer matrix from state(x) to observation(z)
  * @param B	transfer matrix from input(u) to its effect on state (Bu)
  */
 Kalman(Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B);
 
 /**
  * @brief 	constructor for kalman filter with n dimensions
  *
  * @param F	transfer matrix from state(x_k) to the state in the next time frame(x_k+1)
  * @param H	transfer matrix from state(x) to observation(z)
  * @param B	transfer matrix from input(u) to its effect on state(Bu)
  * @param Q	covariance matrix for process noise(w)
  * @param R	covariance matrix for observation noise(v)
  */
 Kalman(Eigen::MatrixXf F, Eigen::MatrixXf H, Eigen::MatrixXf B, Eigen::MatrixXf Q, Eigen::MatrixXf R);

/**
 * @brief	 initialization for x(input) and P(covariance of input), without Init, x is set to a zero vector and P to an identity matrix.
 */ 
 void SetInit(Eigen::VectorXf x, Eigen::MatrixXf P);

/**
 * @brief 	get the closest estimation for the system state without observation, P(Covariance) will not be updated.
 * 
 * @param input		input of the system
 *
 * @return 		estimation of the system state
 */
 Eigen::VectorXf GetEstimate(Eigen::VectorXf input);

/**
 * @brief 	get the closest estimation for the system state without observation, P(Covariance) will not be updated. Used when the input is zero.
 *
 * @return 	estimation of the system state
 */
 Eigen::VectorXf GetEstimate(void);

/**
 * @brief 	processing update of the kalman filter
 *
 * @param input 	input of the system (u)
 */ 
 void Predict(Eigen::VectorXf input);

/**
 * @brief       processing update of the kalman filter, when input is zero
 */
 void Predict(void);

/**
 * @brief 	measure update of the kalman filter
 * 
 * @param observation 	observation (z)
 */
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

