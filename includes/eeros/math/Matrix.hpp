#ifndef ORG_EEROS_MATH_MATRIX_HPP_
#define ORG_EEROS_MATH_MATRIX_HPP_

#include <eeros/core/EEROSException.hpp>

#include <sstream>
#include <cstdlib>
#include <cmath>
#include <stdint.h>

namespace eeros {
	namespace math {
		template < uint8_t N, uint8_t M = 1, typename T = double >
		class Matrix {
		public:
			Matrix() { }
			
			/********** Functions for initializing the matrix **********/
			
			void zero() {
				for(uint8_t i = 0; i < N * M; i++) {
					value[i] = 0;
				}
			}
			
			void eye() {
				zero();
				uint8_t j = (N < M) ? N : M;
				for(uint8_t i = 0; i < j; i++) {
					(*this)(i, i) = 1;
				}
			}
			
			void rotx(double angle) {
				Matrix<N,M,T>& m = *this;
				if(N == 3 && M == 3) {
					m(0,0) = 1;
					m(1,0) = 0;
					m(2,0) = 0;

					m(0,1) = 0;
					m(1,1) = std::cos(angle);
					m(2,1) = std::sin(angle);

					m(0,2) = 0;
					m(1,2) = -std::sin(angle);
					m(2,2) = std::cos(angle);
				}
				else {
					throw EEROSException("rotx(double) is only implemented for 3x3 matrices");
				}
			}
			
			void roty(double angle) {
				Matrix<N,M,T>& m = *this;
				if(N == 3 && M == 3) {
					m(0,0) = std::cos(angle);
					m(1,0) = 0;
					m(2,0) = -std::sin(angle);

					m(0,1) = 0;
					m(1,1) = 1;
					m(2,1) = 0;

					m(0,2) = std::sin(angle);
					m(1,2) = 0;
					m(2,2) = std::cos(angle);
				}
				else {
					throw EEROSException("roty(double) is only implemented for 3x3 matrices");
				}
			}
			
			void rotz(double angle) {
				Matrix<N,M,T>& m = *this;
				if(N == 3 && M == 3) {
					m(0,0) = std::cos(angle);
					m(1,0) = std::sin(angle);
					m(2,0) = 0;

					m(0,1) = -std::sin(angle);
					m(1,1) = std::cos(angle);
					m(2,1) = 0;

					m(0,2) = 0;
					m(1,2) = 0;
					m(2,2) = 1;
				}
				else {
					throw EEROSException("rotz(double) is only implemented for 3x3 matrices");
				}
			}
			
			/********** Functions for checking the matrix characteristics **********/
			
			bool isOrthogonal() const {
				Matrix<N,M,T> result,transposed,eye;
				eye.eye();
				transposed = transpose();
				result = (*this)*transposed;
				return result == eye;
			}
			
			bool isSymmetric() const {
				return (*this) == transpose();
			}
			
			bool isDiagonal() const {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						if(n!=m){
							if(v(n,m)!=0){
								return false;
							}
						}
					}
				}
				return true;
			}
			
			bool isLowerTriangular() const {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						if(n<m){
							if(v(n,m)!=0){
								return false;
							}
						}
					}
				}
				return true;
			}
			
			bool isUpperTriangular() const {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						if(n>m){
							if(v(n,m)!=0){
								return false;
							}
						}
					}
				}
				return true;
			}
			
			bool isInvertible() const {
				// non square matrices can't be inverted, if the determinat of a matrix is zero it is not invertible
				return (N == M) && (this->det() != 0);
			}
			
			/********** Functions for calculating some characteristics of the matrix **********/
			
			uint8_t getNrOfRows() const {
				return N;
			}
			
			uint8_t getNrOfColums() const {
				return M;
			}
			
			uint32_t rank() const {
				uint32_t numberOfNonZeroRows = 0;
				uint32_t i = 0;
				Matrix<N,M,T> matrix;
				for (uint8_t n = 0; n < N; n++) {
					for (uint8_t m = 0; m < M; m++) {
						matrix(n,m) = v(n,m);
					}
				}
				matrix.gaussRowElimination();
				for (uint8_t n = 0; n < N; n++) {
					for (uint8_t m = 0; m < M; m++) {
						if(matrix(n,m)!=0){
							numberOfNonZeroRows++;
							break;
						}    
					}
				}
				return numberOfNonZeroRows;
			}
			
			T det() const {
				if (N==M) {
					if(N==2) { // 2x2 Matrix
						return (v(0,0)*v(1,1)-v(0,1)*v(1,0));
					}
					else if(N==3) { // 3x3 Matrix
						T det = 0; 
						det = v(0, 0) * v(1, 1) * v(2, 2) +
						v(0, 1) * v(1, 2) * v(2, 0) +
						v(0, 2) * v(1, 0) * v(2, 1) -
						v(0, 2) * v(1, 1) * v(2, 0) -
						v(0, 0) * v(1, 2) * v(2, 1) -
						v(0, 1) * v(1, 0) * v(2, 2);
						return det;
					}
					else {
						//use recurcive laplace formula to calculate this
						//for big matrices this methods needs a lot of time this could be improved with another algorithm
						T det = 0;
						uint8_t  ignoredRow = 0;
						for (uint8_t n = 0; n < N; n++) {
							Matrix<N-1,M-1,T> smallerPart;
							uint8_t x = 0,y = 0;
							uint8_t b = 1,a = 0;
							while(a < N) {
								while(b < M) {
									if(a != ignoredRow) {
										smallerPart(y,x) = v(a,b);
										x++;
									}
									b++;
								}
								b = 1;
								x = 0;
								if (a != ignoredRow) {
									y++; 
								}
								a++;
							}
							ignoredRow++;
							T detSmallerPart = smallerPart.det();
							if(n%2 == 0) { // even
								det = det + v(n,0)*detSmallerPart;
							}
							else { // odd
								det = det - v(n,0)*detSmallerPart; 
							}
						}
						return det;
					}
				}
				else {
					throw EEROSException("Calculating determinant failed: Matrix must be square"); 
				}
				return 0;
			}
			
			T trace() const {
				T result = 0;
				uint8_t j = (N < M) ? N : M;
				for(uint8_t i = 0; i < j; i++) {
					result += v(i,i);
				}
				return result;
			}
			
			Matrix<M,N,T> transpose() const {
				Matrix<M,N,T> result;
				Matrix<N-1,M-1,T> smallerPart;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(m,n) = (*this)(n,m);
					}
				}
				return result;
			}
			
			/********** Operators **********/
			
			Matrix<N,M,T>& operator= (T right) {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						v(m,n) = right;
					}
				}
				return *this;
			}
			
			T& operator()(uint8_t n, uint8_t m = 0) {
				if(n >= 0 && n < N && m >= 0 && m < M) {
					return value[M * n + m];
				}
				else {
					std::stringstream msg;
					msg << "Access to element failed: Index out of bound: n = " << n << ", m = "<< m;
					throw EEROSException(msg.str());
				}
			}

			const T operator()(uint8_t n, uint8_t m = 0) const {
				if(n >= 0 && n < N && m >= 0 && m < M) {
					return value[M * n + m];
				}
				else {
					std::stringstream msg;
					msg << "Access to element failed: Index out of bound: n = " << n << ", m = "<< m;
					throw EEROSException(msg.str());
				}
			}

			template < uint8_t K >
			Matrix<N,K,T> operator*(const Matrix<M,K,T> right) const {
				Matrix<N,K,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t k = 0; k < K; k++) {
						result(n, k) = 0;
						for(uint8_t m = 0; m < M; m++) {
							result(n,k) += (*this)(n, m) * right(m, k);
						}
					}
				}
				return result;
			}
			
			Matrix<N,M,T> operator*(T right) const {
				Matrix<N,M,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) * right;
					}
				}
				return result;
			}
			
			Matrix<N,M,T> operator+(const Matrix<N,M,T> right) const {
				Matrix<N,M,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) + right(n,m);
					}
				}
				return result;
			}
			
			Matrix<N,M,T> multiplyElementWise(const Matrix<N,M,T> right) const {
			    Matrix<N,M,T> result;
			    for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) * right(n,m);
					}
			    }
			    return result;
			}
			
			Matrix<N,M,T> operator+(const T right) const {
				Matrix<N,M,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) + right;
					}
				}
				return result;
			}

			Matrix<N,M,T> operator-(const Matrix<N,M,T> right) const {
				Matrix<N,M,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) - right(n,m);
					}
				}
				return result;
			}
			
			Matrix<N,M,T> operator/(T right) const {
				Matrix<N,M,T> result;
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						result(n,m) = (*this)(n,m) / right;
					}
				}
				return result;
			}

			bool operator==(const Matrix<N,M,T> right) const {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						if(v(n,m) != right(n,m))
							return false;
					}
				}
				return true;
			}

			bool operator!=(const Matrix<N,M,T> right) const {
				for(uint8_t n = 0; n < N; n++) {
					for(uint8_t m = 0; m < M; m++) {
						if(v(n,m) != right(n,m)) {
							return true;
						}
					}
				}
				return false;
			}
			
			Matrix<N,M,T> operator!() const {
				T determinant = det();
				if(N!=M) {
					throw EEROSException("Invert failed: matrix not square");
				}
				else if(determinant == 0) {
					throw EEROSException("Invert failed: determinat of matrix is 0");
				}
				else if(isOrthogonal() == true) {
					return transpose();
				}
				else if(N == 3) {
					Matrix<N,M,T> result;
					result(0, 0) = v(1, 1) * v(2, 2) - v(1, 2) * v(2, 1);
					result(1, 0) = v(1, 2) * v(2, 0) - v(1, 0) * v(2, 2);
					result(2, 0) = v(1, 0) * v(2, 1) - v(1, 1) * v(2, 0);
					result(0, 1) = v(0, 2) * v(2, 1) - v(0, 1) * v(2, 2);
					result(1, 1) = v(0, 0) * v(2, 2) - v(0, 2) * v(2, 0);
					result(2, 1) = v(0, 1) * v(2, 0) - v(0, 0) * v(2, 1);
					result(0, 2) = v(0, 1) * v(1, 2) - v(0, 2) * v(1, 1);
					result(1, 2) = v(0, 2) * v(1, 0) - v(0, 0) * v(1, 2);
					result(2, 2) = v(0, 0) * v(1, 1) - v(0, 1) * v(1, 0);
					return (result / determinant);
				}
				else if(N == 2) {
					Matrix<N,M,T> result;
					result(0, 0) = v(1, 1);
					result(1, 0) = -v(1, 0);
					result(0, 1) = -v(0, 1);
					result(1, 1) = v(0, 0);
					return (result / determinant);
				}
				else {
					// This algorithm needs a lot of time maybe this could be replaced
					Matrix<N,M,T> result;
					Matrix<N-1,M-1,T> smallerPart;
					uint8_t ignoredRow = 0;
					uint8_t ignoredColum = 0;
					uint8_t a = 0;
					uint8_t b = 0;
					for(uint8_t n = 0; n < N; n++) {
						for(uint8_t m = 0; m < M; m++) {
							uint8_t a = 0;
							uint8_t b = 0;
							// 1.Create "matrix of minors"
							for(uint8_t u = 0; u < N; u++) {
								for(uint8_t w = 0; w < M; w++) {
									if(u != n && w != m){
										smallerPart(a,b) = v(u,w);
										b++;
										b = b%(M-1);
									}
								}
								if(u != n) {
									a++;
									a = a&(N-1);
								}
							}
							// 2.Swapp signs 
							if(n%2 == 0) {
								if(m%2 != 0) {
									result(n,m) = -smallerPart.det();
								}
								else {
									result(n,m) = smallerPart.det();
								}
							}
							else {
								if(m%2 != 0){
									result(n,m) = smallerPart.det();
								}
								else {
									result(n,m) = -smallerPart.det();
								}
							}
							// 3.Divide trhoug det of the original matrix
							result(n,m) = result(n,m)/determinant;
						}
					}
					// 4. transpose
					return result.transpose();
				}
			}

			operator T () const {
				if(N == 1 && M == 1)
					return value[0];
				else 
					throw EEROSException("Dimension is not 1x1");
			}

			/********** Static functions **********/
			
			static Matrix<N,M,T> createRotX(double angle) {
				Matrix<N,M,T> m;
				m.rotx(angle);
				return m;
			}
			
			static Matrix<N,M,T> createRotY(double angle) {
				Matrix<N,M,T> m;
				m.roty(angle);
				return m;
			}
			
			static Matrix<N,M,T> createRotZ(double angle) {
				Matrix<N,M,T> m;
				m.rotz(angle);
				return m;
			}
			
			static Matrix<2,1,T> createVector2(T x, T y) {
				Matrix<2,1,T> v;
				v(0) = x;
				v(1) = y;
				return v;
			}
			
			static Matrix<3,1,T> createVector3(T x, T y, T z) {
				Matrix<3,1,T> v;
				v(0) = x;
				v(1) = y;
				v(2) = z;
				return v;
			}
			
			
			static Matrix<3,1,T> crossProduct(Matrix<3,1,T> a,Matrix<3,1,T> b) {
			    Matrix<3,1,T> result;
			    result(0,0) = a(1,0)*b(2,0) - a(2,0)*b(1,0);
			    result(1,0) = a(2,0)*b(0,0) - a(0,0)*b(2,0);
			    result(2,0) = a(0,0)*b(1,0) - a(1,0)*b(0,0); 
			    return result;
			}
			
			static Matrix<3,3,T> createSkewSymmetricMatrix(Matrix<3,1,T> a) {
			    Matrix<3,3,T> result;
			    result(0,0) = 0; result(0,1) = -a(2); result(0,2) = a(1);
			    result(1,0) = a(2); result(1,1) = 0; result(1,2) = -a(0);
			    result(2,0) = -a(1); result(2,1) = a(0); result(2,2) = 0;
			    return result;
			}
			
			void gaussRowElimination() {
				uint8_t completedColum = 0;
				uint8_t completedRow = 0;
				uint8_t checkingRow = 0;
				uint8_t rootRow = 0;
				T rowFactor = 0;
				
				sortForGaussAlgorithm();
				while(completedColum < M) {
					rootRow = completedRow;
					checkingRow = rootRow + 1;
					while(checkingRow < N && v(rootRow,completedColum) != 0) {
						if(v(checkingRow,completedColum) != 0) {
							rowFactor = v(checkingRow,completedColum)/v(rootRow,completedColum); 
							for(uint8_t m = completedColum; m < M; m++) {
								(*this)(checkingRow,m) = v(checkingRow,m) - rowFactor*v(rootRow,m);
							}
						}
						checkingRow++;
					}
					completedRow++;
					completedColum++;
				}
			}
			
			void  sortForGaussAlgorithm() {
				uint8_t completedColum = 0;
				uint8_t completedRow = 0;
				uint8_t swapRow = completedRow + 1;
				
				while(completedColum < M) {
					while(completedRow < N) {
						if(v(completedRow,completedColum) == 0 && swapRow < N && completedRow <N-1) {
							swapRows(completedRow,swapRow);
							swapRow++;
						}
						else {
							completedRow++; 
						}
				  }
				  completedColum++;
				  swapRow = completedRow + 1;
				}
			}
			
			void swapRows(uint8_t row1, uint8_t row2) {
				Matrix<1,M,T> temp;
				temp.zero();
				for(uint8_t m = 0; m < M; m++) {
					temp(0,m) = v(row1,m);
					(*this)(row1,m) = v(row2,m);
					(*this)(row2,m) =  temp(0,m);
				}
			}
			
		private:
			T value[ N * M ];
			
			T& v(uint8_t n, uint8_t m = 0) {
				if (n >= 0 && n < N && m >= 0 && m < M)
					return value[M * n + m ];
				else {
					std::stringstream msg;
					msg << "v(): Index out of bound: n = " << n << ", m = "<< m;
					throw EEROSException(msg.str());
				}
			}
			
			const T& v(uint8_t n, uint8_t m = 0) const {
				if (n >= 0 && n < N && m >= 0 && m < M)
					return value[M * n + m];
				else {
					std::stringstream msg;
					msg << "v(): Index out of bound: n = " << n << ", m = "<< m;
					throw EEROSException(msg.str());
				}
			}
					
			
		}; // END class Matrix
		
		/********** Operators **********/
		
		template < uint8_t N, uint8_t M = 1, typename T = double >
		Matrix<N,M,T> operator*(T left, Matrix<N,M,T> right) {
			Matrix<N,M,T> result;
			for(uint8_t n = 0; n < N; n++) {
				for(uint8_t m = 0; m < M; m++) {
					result(n,m) = right(n,m) * left;
				}
			}
			return result;
		}
		
		typedef Matrix<2,1> Vector2;
		typedef Matrix<3,1> Vector3;
		typedef Matrix<4,1> Vector4;
		
	} // END namespace math
} // END namespache eeros

#endif /* ORG_EEROS_MATH_MATRIX_HPP_ */
