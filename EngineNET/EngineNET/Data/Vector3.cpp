namespace EngineNET
{
	static Vector3::Vector3()
	{
		Zero = gcnew Vector3();
		One = gcnew Vector3(1.0f, 1.0f, 1.0f);
		Up = gcnew Vector3(0.0f, 1.0f, 0.0f);
		Forward = gcnew Vector3(0.0f, 0.0f, 1.0f);
		Right = gcnew Vector3(1.0f, 0.0f, 0.0f);
	}

	Vector3::Vector3()
	{
		_pVector3 = new Engine::Vector3();
	}

	Vector3::~Vector3()
	{
		this->!Vector3();
	}

	Vector3::!Vector3()
	{
		delete _pVector3;
	}

	Vector3::Vector3(Vector2^ rhs)
	{
		if (_pVector3 != nullptr)
		{
			delete _pVector3;
		}

		_pVector3->X = rhs->X;
		_pVector3->Y = rhs->Y;
		_pVector3->Z = 0.0f;
	}

	Vector3::Vector3(Engine::Vector3& vec)
	{
		_pVector3 = new Engine::Vector3();
		_pVector3->X = vec.X;
		_pVector3->Y = vec.Y;
		_pVector3->Z = vec.Z;
	}

#pragma managed(push, off)
	float* Operation(Engine::Vector3* lhs, Engine::Vector3* rhs, int op)
	{
		Engine::Vector3 out;
		switch (op)
		{
		case 0: // Add
			out = lhs->operator+(*rhs);
			break;
		case 1: // Sub
			out = lhs->operator-(*rhs);
			break;
		case 2: // Mul
			out = lhs->operator*(*rhs);
			break;
		case 3: // Div
			out = lhs->operator/(*rhs);
			break;
		}

		return new float[3] { out.X, out.Y, out.Z };
	}

	float* Operation(Engine::Vector3* lhs, float rhs, int op)
	{
		Engine::Vector3 out;
		switch (op)
		{
		case 0: // Mul
			out = lhs->operator*(rhs);
			break;
		case 1: // Div
			out = lhs->operator/(rhs);
			break;
		}

		return new float[3]{ out.X, out.Y, out.Z };
	}
#pragma managed(pop)

	Vector3::Vector3(Vector3^ rhs)
	{
		Engine::Vector3* vec = rhs->_pVector3;
		_pVector3 = new Engine::Vector3(vec->X, vec->Y, vec->Z);
	}

	Vector3::Vector3(float x, float y, float z)
	{
		_pVector3 = new Engine::Vector3(x, y, z);
	}

	Vector3::Vector3(int x, int y, int z)
	{
		_pVector3 = new Engine::Vector3(x, y, z);
	}

	Vector3^ Vector3::operator+(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 0);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector3^ Vector3::operator*(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 2);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector3^ Vector3::operator-(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 1);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector3^ Vector3::operator/(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 3);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector3^ Vector3::operator*(float rhs)
	{
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		float* out = Operation(lhsVec, rhs, 0);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	Vector3^ Vector3::operator/(float rhs)
	{
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		float* out = Operation(lhsVec, rhs, 1);
		Vector3^ outVec = gcnew Vector3(out[0], out[1], out[2]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	void Vector3::operator=(Vector3^ rhs)
	{
		delete _pVector3;
		Engine::Vector3* vec = rhs->_pVector3;
		_pVector3 = new Engine::Vector3(vec->X, vec->Y, vec->Z);
	}

	Vector3::operator Engine::Vector3*()
	{
		return _pVector3;
	}

	Vector3^ Vector3::operator+=(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 0);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector3^ Vector3::operator*=(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 2);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector3^ Vector3::operator-=(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 1);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector3^ Vector3::operator/=(Vector3^ rhs)
	{
		Engine::Vector3* ref = rhs->_pVector3;
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		Engine::Vector3* rhsVec = new Engine::Vector3(ref->X, ref->Y, ref->Z);
		float* out = Operation(lhsVec, rhsVec, 3);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector3^ Vector3::operator*=(float rhs)
	{
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		float* out = Operation(lhsVec, rhs, 0);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;

		return this;
	}

	Vector3^ Vector3::operator/=(float rhs)
	{
		Engine::Vector3* lhsVec = new Engine::Vector3(_pVector3->X, _pVector3->Y, _pVector3->Z);
		float* out = Operation(lhsVec, rhs, 1);
		X = out[0];
		Y = out[1];
		Z = out[2];
		delete[] out;
		delete lhsVec;

		return this;
	}

	float Vector3::X::get()
	{
		return _pVector3->X;
	}

	void Vector3::X::set(float v)
	{
		_pVector3->X = v;
	}

	float Vector3::Y::get()
	{
		return _pVector3->Y;
	}

	void Vector3::Y::set(float v)
	{
		_pVector3->Y = v;
	}

	float Vector3::Z::get()
	{
		return _pVector3->Z;
	}

	void Vector3::Z::set(float v)
	{
		_pVector3->Z = v;
	}
}