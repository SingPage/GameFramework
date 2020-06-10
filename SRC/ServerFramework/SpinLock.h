#pragma once

class CSpinLock {
public:
	CSpinLock() : m_LockFlag(ATOMIC_FLAG_INIT){
	}

	virtual				~CSpinLock() {

	}

	VOID				Lock() {

		while (m_LockFlag.test_and_set()) {

		}
	}

	BOOL				TryLock() {

		return m_LockFlag.test_and_set();
	}

	VOID				UnLock() {

		m_LockFlag.clear();
	}

private:

	std::atomic_flag	m_LockFlag;

};