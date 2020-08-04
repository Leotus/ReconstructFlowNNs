#include "CoordTool.h"

bool CoordTool::volidateCoord(int x, int y, int z,int dims[3]) {
	if (x < dims[0] - 1 && y < dims[1] - 1 && z < dims[2] - 1) {
		if (x >= 0 && y >= 0 && z >= 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void CoordTool::initCoord(int x, int y, int z, vector<int*>* coord) {
	coord->clear();
	for (int i = 0; i <= 1; i++) {
		for (int j = 0; j <= 1; j++) {
			for (int k = 0; k <= 1; k++) {
				coord->push_back(new int[3]{ x - 1 + i,y - 1 + j,z - 1 + k });
			}
		}
	}
}

void CoordTool::appendCoord(int x, int y, int z, int a, vector<int*>* coord,int dims[3]) {
	coord->clear();
	// �ϱ��� Y = y-1-aƽ��
	for (int ix = x - 1 - a; ix <= x + a; ix++) {
		for (int iz = z - 1 - a; iz <= z + a; iz++) {
			if (CoordTool::volidateCoord(ix, y - 1 - a, iz,dims)) {
				coord->push_back(new int[3]{ ix,y - 1 - a, iz });
			}
		}
	}
	// �±��� Y = y+aƽ��
	for (int ix = x - 1 - a; ix <= x + a; ix++) {
		for (int iz = z - 1 - a; iz <= z + a; iz++) {
			if (CoordTool::volidateCoord(ix, y + a, iz,dims)) {
				coord->push_back(new int[3]{ ix,y + a, iz });
			}
		}
	}
	// ����� X = x-1-aƽ��
	for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//ͬʱ�޳��������±�����غ���
		for (int iz = z - 1 - a; iz <= z + 1; iz++) {
			if (CoordTool::volidateCoord(x - 1 - a, iy, iz,dims)) {
				coord->push_back(new int[3]{ x - 1 - a,iy, iz });
			}
		}
	}
	// �ұ��� X = x+aƽ��
	for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//ͬʱ�޳��������±�����غ���
		for (int iz = z - 1 - a; iz <= z + 1; iz++) {
			if (CoordTool::volidateCoord(x + a, iy, iz,dims)) {
				coord->push_back(new int[3]{ x + a,iy, iz });
			}
		}
	}
	// �����Z = z-1-aƽ��
	for (int ix = x - 1 - a + 1; ix <= x + a - 1; ix++) {//�޳������ұ�����غ���
		for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//�޳������±�����غ���
			if (CoordTool::volidateCoord(ix, iy, z - 1 - a,dims)) {
				coord->push_back(new int[3]{ ix,iy, z - 1 - a });
			}
		}
	}
	// ǰ����Z = z+aƽ��
	for (int ix = x - 1 - a + 1; ix <= x + a - 1; ix++) {//�޳������ұ�����غ���
		for (int iy = y - 1 - a + 1; iy <= y + a - 1; iy++) {//�޳������±�����غ���
			if (CoordTool::volidateCoord(ix, iy, z + a,dims)) {
				coord->push_back(new int[3]{ ix,iy, z + a });
			}
		}
	}
}
