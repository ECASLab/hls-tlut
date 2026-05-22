layers_ = {
    std::make_shared<QLayer<ap_fixed<kQT[0].first, kQT[0].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[1].first, kQT[1].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[2].first, kQT[2].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[3].first, kQT[3].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[4].first, kQT[4].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[5].first, kQT[5].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[6].first, kQT[6].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[7].first, kQT[7].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[8].first, kQT[8].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[9].first, kQT[9].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[10].first, kQT[10].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[11].first, kQT[11].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[12].first, kQT[12].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[13].first, kQT[13].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[14].first, kQT[14].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[15].first, kQT[15].second>, Padding>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[16].first, kQT[16].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[17].first, kQT[17].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[18].first, kQT[18].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[19].first, kQT[19].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[20].first, kQT[20].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[21].first, kQT[21].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[22].first, kQT[22].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[23].first, kQT[23].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[24].first, kQT[24].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[25].first, kQT[25].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[26].first, kQT[26].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[27].first, kQT[27].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[28].first, kQT[28].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[29].first, kQT[29].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[30].first, kQT[30].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[31].first, kQT[31].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[32].first, kQT[32].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[33].first, kQT[33].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[34].first, kQT[34].second>, Layerwise>>(),
    std::make_shared<QLayer<ap_fixed<kQT[35].first, kQT[35].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[36].first, kQT[36].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[37].first, kQT[37].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[38].first, kQT[38].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[39].first, kQT[39].second>, Padding>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[40].first, kQT[40].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[41].first, kQT[41].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[42].first, kQT[42].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[43].first, kQT[43].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[44].first, kQT[44].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[45].first, kQT[45].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[46].first, kQT[46].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[47].first, kQT[47].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[48].first, kQT[48].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[49].first, kQT[49].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[50].first, kQT[50].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[51].first, kQT[51].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[52].first, kQT[52].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[53].first, kQT[53].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[54].first, kQT[54].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[55].first, kQT[55].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[56].first, kQT[56].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[57].first, kQT[57].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[58].first, kQT[58].second>, Layerwise>>(),
    std::make_shared<QLayer<ap_fixed<kQT[59].first, kQT[59].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[60].first, kQT[60].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[61].first, kQT[61].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[62].first, kQT[62].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[63].first, kQT[63].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[64].first, kQT[64].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[65].first, kQT[65].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[66].first, kQT[66].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[67].first, kQT[67].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[68].first, kQT[68].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[69].first, kQT[69].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[70].first, kQT[70].second>, Layerwise>>(),
    std::make_shared<QLayer<ap_fixed<kQT[71].first, kQT[71].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[72].first, kQT[72].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[73].first, kQT[73].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[74].first, kQT[74].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[75].first, kQT[75].second>, Padding>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[76].first, kQT[76].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[77].first, kQT[77].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[78].first, kQT[78].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[79].first, kQT[79].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[80].first, kQT[80].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[81].first, kQT[81].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[82].first, kQT[82].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[83].first, kQT[83].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[84].first, kQT[84].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[85].first, kQT[85].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[86].first, kQT[86].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[87].first, kQT[87].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[88].first, kQT[88].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[89].first, kQT[89].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[90].first, kQT[90].second>, Mapper>>(),
    std::make_shared<QLayer<ap_fixed<kQT[91].first, kQT[91].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[92].first, kQT[92].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[93].first, kQT[93].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[94].first, kQT[94].second>, Layerwise>>(),
    std::make_shared<QLayer<ap_fixed<kQT[95].first, kQT[95].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[96].first, kQT[96].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[97].first, kQT[97].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[98].first, kQT[98].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[99].first, kQT[99].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[100].first, kQT[100].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[101].first, kQT[101].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[102].first, kQT[102].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[103].first, kQT[103].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[104].first, kQT[104].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[105].first, kQT[105].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[106].first, kQT[106].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[107].first, kQT[107].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[108].first, kQT[108].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[109].first, kQT[109].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[110].first, kQT[110].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[111].first, kQT[111].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[112].first, kQT[112].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[113].first, kQT[113].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[114].first, kQT[114].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[115].first, kQT[115].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[116].first, kQT[116].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[117].first, kQT[117].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[118].first, kQT[118].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[119].first, kQT[119].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[120].first, kQT[120].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[121].first, kQT[121].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[122].first, kQT[122].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[123].first, kQT[123].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[124].first, kQT[124].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[125].first, kQT[125].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[126].first, kQT[126].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[127].first, kQT[127].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[128].first, kQT[128].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[129].first, kQT[129].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[130].first, kQT[130].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[131].first, kQT[131].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[132].first, kQT[132].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[133].first, kQT[133].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[134].first, kQT[134].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[135].first, kQT[135].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[136].first, kQT[136].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[137].first, kQT[137].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[138].first, kQT[138].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[139].first, kQT[139].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[140].first, kQT[140].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[141].first, kQT[141].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[142].first, kQT[142].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[143].first, kQT[143].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[144].first, kQT[144].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[145].first, kQT[145].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[146].first, kQT[146].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[147].first, kQT[147].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[148].first, kQT[148].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[149].first, kQT[149].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[150].first, kQT[150].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[151].first, kQT[151].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[152].first, kQT[152].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[153].first, kQT[153].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[154].first, kQT[154].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[155].first, kQT[155].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[156].first, kQT[156].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[157].first, kQT[157].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[158].first, kQT[158].second>, Padding>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[159].first, kQT[159].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[160].first, kQT[160].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[161].first, kQT[161].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[162].first, kQT[162].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[163].first, kQT[163].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[164].first, kQT[164].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[165].first, kQT[165].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[166].first, kQT[166].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[167].first, kQT[167].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[168].first, kQT[168].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[169].first, kQT[169].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[170].first, kQT[170].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[171].first, kQT[171].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[172].first, kQT[172].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[173].first, kQT[173].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[174].first, kQT[174].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[175].first, kQT[175].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[176].first, kQT[176].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[177].first, kQT[177].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[178].first, kQT[178].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[179].first, kQT[179].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[180].first, kQT[180].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[181].first, kQT[181].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[182].first, kQT[182].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[183].first, kQT[183].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[184].first, kQT[184].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[185].first, kQT[185].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[186].first, kQT[186].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[187].first, kQT[187].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[188].first, kQT[188].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[189].first, kQT[189].second>, Layerwise>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[190].first, kQT[190].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[191].first, kQT[191].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[192].first, kQT[192].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[193].first, kQT[193].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[194].first, kQT[194].second>, DepthConv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[195].first, kQT[195].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[196].first, kQT[196].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[197].first, kQT[197].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[198].first, kQT[198].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[199].first, kQT[199].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[200].first, kQT[200].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[201].first, kQT[201].second>, Conv2D>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[202].first, kQT[202].second>, Multiplier>>(),
    std::make_shared<QLayer<ap_fixed<kQT[203].first, kQT[203].second>, Add>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[204].first, kQT[204].second>, Mapper>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[205].first, kQT[205].second>, Pooling>>(),
    std::make_shared<
        QLayer<ap_fixed<kQT[206].first, kQT[206].second>, Dense>>(),
    std::make_shared<QLayer<ap_fixed<kQT[207].first, kQT[207].second>, Add>>(),
    std::make_shared<QLayer<ap_fixed<kQT[208].first, kQT[208].second>,
                            ReductionActivation>>(),
};
