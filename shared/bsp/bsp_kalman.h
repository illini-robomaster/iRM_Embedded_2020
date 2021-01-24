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


/**
 * bsp_kalman.h starts
 */
namespace bsp{

/**
 * All the symbols stand the same meaning as the wikipedia page for Kalman filter
 */
template <int xDims, int yDims, int uDims> 
// State Dims (x), Observation Dims (y), and Input Dims(u)
class Kalman {
 public:
 /**
  * @brief 		constructor for kalman filter with n dimensions
  *
  * @param F		transfer matrix from state(x_k) to the state in the next time frame(x_k+1)
  * @param H		transfer matrix from state(x) to observation(z)
  * @param B		transfer matrix from input(u) to its effect on state(Bu)
  * @param Q		covariance matrix for process noise(w)
  * @param R		covariance matrix for observation noise(v)
  */
 Kalman(const Eigen::Matrix<float, xDims, xDims>& F,
 	const Eigen::Matrix<float, yDims, xDims>& H,
	const Eigen::Matrix<float, xDims, uDims>& B, 
	const Eigen::Matrix<float, xDims, xDims>& Q
		= Eigen::MatrixXf::Identity(xDims, xDims), 
	const Eigen::Matrix<float, yDims, yDims>& R
		= Eigen::MatrixXf::Identity(yDims, yDims));

/**
 * @brief		 initialization for x(input) and P(covariance of input), without Init, x is set to a zero vector and P to an identity matrix.
 */ 
 void SetInit(const Eigen::Matrix<float, xDims, 1>& x, 
	      const Eigen::Matrix<float, xDims, xDims>& P);

/**
 * @brief 		get the closest estimation for the system state without observation, P(Covariance) will not be updated.
 * 
 * @param   input	input of the system, default to be Zero. doesn't update covariance
 *
 * @return 		estimation of the system state
 */
 Eigen::Matrix<float, xDims, 1>& EstimateNext(const Eigen::Matrix<float, uDims, 1>& input
			= Eigen::VectorXf::Zero(uDims));

/**
 * @brief 		processing update of the kalman filter, update covariance.
 *
 * @param input 	input of the system (u), default to be Zero
 */ 
 void Predict(const Eigen::Matrix<float, uDims, 1>& input
			= Eigen::VectorXf::Zero(uDims));

/**
 * @brief 		measure update of the kalman filter
 * 
 * @param observation 	observation (z)
 */
 void Update(const Eigen::Matrix<float, yDims, 1>& observation);

/**
 * @brief		processing update and measure update.
 *
 * @param observation 	observation (z)
 * @param input 	input of the system (u), default to be Zero
 */
 void Process(	const Eigen::Matrix<float, yDims, 1>& observation,
		const Eigen::Matrix<float, uDims, 1>& input
		= Eigen::VectorXf::Zero(uDims));

/**
 * @brief 		getter to private variable x
 *
 * @return 		system current state x
 */
 const Eigen::Matrix<float, xDims, 1>& GetState(void) const;


 private:
 Eigen::Matrix<float, xDims, 1> x_;    // estimation in wiki Kalman filter formula
 Eigen::Matrix<float, xDims, xDims> P_;    // covariance in wiki Kalman filter formula
 
 const Eigen::Matrix<float, xDims, xDims> F_;    // transfer matrix: x_k -> x_k+1
 const Eigen::Matrix<float, yDims, xDims> H_;    // transfer matrix: x_k -> observation z
 const Eigen::Matrix<float, xDims, uDims> B_;    // transfer matrix: input u -> x

 const Eigen::Matrix<float, xDims, xDims> Q_;    // noise covariance for predict
 const Eigen::Matrix<float, yDims, yDims> R_;    // noise covariance for measure
};

} // namespace bsp ends

/**
 * bsp_kalman.h ends
 */


/**
 * bsp_kalman.cpp starts
 */

namespace bsp {
template <int xDims, int yDims, int uDims>
Kalman<xDims, yDims, uDims>::Kalman( 
		const Eigen::Matrix<float, xDims, xDims>& F,
 		const Eigen::Matrix<float, yDims, xDims>& H,
		const Eigen::Matrix<float, xDims, uDims>& B, 
		const Eigen::Matrix<float, xDims, xDims>& Q,
		const Eigen::Matrix<float, yDims, yDims>& R )
     : F_(F), H_(H), B_(B), Q_(Q), R_(R) {
 x_.setZero(xDims);
 P_.setIdentity(xDims, xDims);
}

template <int xDims, int yDims, int uDims>
void Kalman<xDims, yDims, uDims>::SetInit(
		const Eigen::Matrix<float, xDims, 1>& x, 
	      	const Eigen::Matrix<float, xDims, xDims>& P) {
 x_ = x;
 P_ = P;
}

template <int xDims, int yDims, int uDims>
Eigen::Matrix<float, xDims, 1>& Kalman<xDims, yDims, uDims>::EstimateNext(
		const Eigen::Matrix<float, uDims, 1>& u) {
 x_ = F_ * x_ + B_ * u;
 return x_;
}

template <int xDims, int yDims, int uDims>
void Kalman<xDims, yDims, uDims>::Predict(
		const Eigen::Matrix<float, uDims, 1>& u) {
 x_ = F_ * x_ + B_ * u;
 P_ = F_ * P_ * F_.transpose() + Q_;
}

template <int xDims, int yDims, int uDims>
void Kalman<xDims, yDims, uDims>::Update(
		const Eigen::Matrix<float, yDims, 1>& y) {
 Eigen::Matrix<float, yDims, yDims> S = H_ * P_ * H_.transpose() + R_;
 Eigen::Matrix<float, xDims, yDims> K = P_ * H_.transpose() * S.inverse();
 x_ = x_ + K * (y - H_ * x_);
 P_ = (Eigen::Matrix<float, xDims, xDims>::Identity(xDims, xDims) - K * H_) * P_;
}

template <int xDims, int yDims, int uDims>
void Kalman<xDims, yDims, uDims>::Process(
		const Eigen::Matrix<float, yDims, 1>& y,
		const Eigen::Matrix<float, uDims, 1>& u) {
 Predict(u);
 Update(y);
}


template <int xDims, int yDims, int uDims>
 const Eigen::Matrix<float, xDims, 1>& Kalman<xDims, yDims, uDims>::GetState(void) const {
 return x_;
}

} // namespace bsp ends


/**
 * bsp_kalman.cpp ends
 */
