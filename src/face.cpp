#include "face.h"

void Face::CalculateNormal(){
    lin_alg::Vector<4> v1_v2_diff = vertices[0]->pos - vertices[1]->pos;
    lin_alg::Vector<4> v1_v3_diff = vertices[0]->pos - vertices[2]->pos;
    normal = lin_alg::CrossProduct(v1_v2_diff, v1_v3_diff).GetAsVector3();
    normal.Normalise();
};