#include "ValidateData.h"

void ValidateData::validateTrainData(vector<vector<map<string, double>*>*>* trdata, vtkPolyData* streamlinedats,int GROUP_NUM)
{
    for (int index = 0; index < trdata->size(); index++) {
        vector<map<string, double>*>* groupd = (*trdata)[index];
        if (groupd->size() == GROUP_NUM) {
            for (int i = 0; i < groupd->size(); i++) {
                map<string, double>* dat = (*groupd)[i];
                double dx = dat->find("dx")->second;
                double dy = dat->find("dy")->second;
                double dz = dat->find("dz")->second;
                double su = dat->find("u")->second;
                double sv = dat->find("v")->second;
                double sw = dat->find("w")->second;
                double nu = dat->find("targetU")->second;
                double nv = dat->find("targetV")->second;
                double nw = dat->find("targetW")->second;
                double nx = dat->find("nx")->second;
                double ny = dat->find("ny")->second;
                double nz = dat->find("nz")->second;

                double sx = nx - dx;
                double sy = ny - dy;
                double sz = nz - dz;

                // Build locator object
                vtkSmartPointer<vtkPointLocator> locator = vtkPointLocator::New();
                locator->SetDataSet(streamlinedats);
                locator->BuildLocator();
                // Define query position
                double pt[3] = { sx,sy,sz };
                // Get the ID of the point that is closest to the query position
                vtkIdType id = locator->FindClosestPoint(pt);
                // Retrieve the first attribute value from this point
                double xfind = streamlinedats->GetPointData()->GetArray("x")->GetComponent(id, 0);
                double yfind = streamlinedats->GetPointData()->GetArray("y")->GetComponent(id, 0);
                double zfind = streamlinedats->GetPointData()->GetArray("z")->GetComponent(id, 0);
                double ufind = streamlinedats->GetPointData()->GetArray("velocity")->GetComponent(id, 0);
                double vfind = streamlinedats->GetPointData()->GetArray("velocity")->GetComponent(id, 1);
                double wfind = streamlinedats->GetPointData()->GetArray("velocity")->GetComponent(id, 2);
                if (i == 1 || i == 4 || i == GROUP_NUM - 1) {
                    cout << " Output data:  ";
                    cout << " dx: " <<
                        dx << " dy: " <<
                        dy << " dz: " <<
                        dz << endl << " nx: " <<
                        nx << " ny: " <<
                        ny << " nz: " <<
                        nz << endl << " su: " <<
                        su << " sv: " <<
                        sv << " sw: " <<
                        sw << endl;
                    cout << " Calculate data: ";
                    cout << " sx:" <<
                        sx << " sy:" <<
                        sy << " sz: " <<
                        sz << endl;
                    cout << " Find data: ";
                    cout << " find-id:" <<
                        id << " find-x:" <<
                        xfind << " find-y: " <<
                        yfind << " find-z: " <<
                        zfind << " find-u:" <<
                        ufind << " find-v: " <<
                        vfind << " find-w: " <<
                        wfind << endl;
                    if (sx == xfind && sy == yfind && sz == zfind && su == ufind && sv == vfind && sw == wfind) {
                        cout << "Validated True" << endl;
                    }
                    else {
                        cout << "Validated False" << endl;
                    }
                }
            }
        }
        else {
            continue;
        }
    }
}