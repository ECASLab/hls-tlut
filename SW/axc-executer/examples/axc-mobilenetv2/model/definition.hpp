auto layer_0 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[0]);
layer_0->activation = Activations::NONE;
layer_0->type = Layers::CONV2D;
layer_0->size = 3 * 3 * 3 * 32 * sizeof(float);
layer_0->data = nullptr;
layer_0->input_size = -1;
layer_0->output_size = -1;
layer_0->num_inputs = 1;
layer_0->num_outputs = 1;
layer_0->props = Conv2D{.num_dimensions = 4,
                        .dimensions = {3, 3, 3, 32},
                        .strides = {2, 2, 1, 1},
                        .padding = {1, 1, 0, 0},
                        .dilatations = {1, 1, 1, 1}};

auto layer_1 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[1]);
layer_1->activation = Activations::NONE;
layer_1->type = Layers::MULTIPLIER;
layer_1->size = 32 * sizeof(float);
layer_1->data = nullptr;
layer_1->input_size = -1;
layer_1->output_size = -1;
layer_1->props = Multiplier{
    .num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_1->num_inputs = 1;
layer_1->num_outputs = 1;
auto layer_2 = std::dynamic_pointer_cast<Layer<Add>>(layers_[2]);
layer_2->activation = Activations::NONE;
layer_2->type = Layers::ADD;
layer_2->size = 32 * sizeof(float);
layer_2->input_size = -1;
layer_2->output_size = -1;
layer_2->data = nullptr;
layer_2->props =
    Add{.num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_2->num_inputs = 1;
layer_2->num_outputs = 1;

auto layer_3 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[3]);
layer_3->activation = Activations::RELU;
layer_3->type = Layers::ACTIVATION;
layer_3->size = 0;
layer_3->data = nullptr;
layer_3->input_size = -1;
layer_3->output_size = -1;
layer_3->props = Mapper{.maxvalue = 6.0};
layer_3->num_inputs = 1;
layer_3->num_outputs = 1;

auto layer_4 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[4]);
layer_4->activation = Activations::NONE;
layer_4->type = Layers::DEPTHWISE_CONV2D;
layer_4->size = 3 * 3 * 32 * 1 * sizeof(float);
layer_4->data = nullptr;
layer_4->input_size = -1;
layer_4->output_size = -1;
layer_4->num_inputs = 1;
layer_4->num_outputs = 1;
layer_4->props = DepthConv2D{.num_dimensions = 3,
                             .dimensions = {3, 3, 32, 1},
                             .strides = {1, 1, 1, 1},
                             .padding = {1, 1, 0, 0},
                             .dilatations = {1, 1, 1, 1}};

auto layer_5 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[5]);
layer_5->activation = Activations::NONE;
layer_5->type = Layers::MULTIPLIER;
layer_5->size = 32 * sizeof(float);
layer_5->data = nullptr;
layer_5->input_size = -1;
layer_5->output_size = -1;
layer_5->props = Multiplier{
    .num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_5->num_inputs = 1;
layer_5->num_outputs = 1;
auto layer_6 = std::dynamic_pointer_cast<Layer<Add>>(layers_[6]);
layer_6->activation = Activations::NONE;
layer_6->type = Layers::ADD;
layer_6->size = 32 * sizeof(float);
layer_6->input_size = -1;
layer_6->output_size = -1;
layer_6->data = nullptr;
layer_6->props =
    Add{.num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_6->num_inputs = 1;
layer_6->num_outputs = 1;

auto layer_7 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[7]);
layer_7->activation = Activations::RELU;
layer_7->type = Layers::ACTIVATION;
layer_7->size = 0;
layer_7->data = nullptr;
layer_7->input_size = -1;
layer_7->output_size = -1;
layer_7->props = Mapper{.maxvalue = 6.0};
layer_7->num_inputs = 1;
layer_7->num_outputs = 1;

auto layer_8 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[8]);
layer_8->activation = Activations::NONE;
layer_8->type = Layers::CONV2D;
layer_8->size = 1 * 1 * 32 * 16 * sizeof(float);
layer_8->data = nullptr;
layer_8->input_size = -1;
layer_8->output_size = -1;
layer_8->num_inputs = 1;
layer_8->num_outputs = 1;
layer_8->props = Conv2D{.num_dimensions = 4,
                        .dimensions = {1, 1, 32, 16},
                        .strides = {1, 1, 1, 1},
                        .padding = {0, 0, 0, 0},
                        .dilatations = {1, 1, 1, 1}};

auto layer_9 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[9]);
layer_9->activation = Activations::NONE;
layer_9->type = Layers::MULTIPLIER;
layer_9->size = 16 * sizeof(float);
layer_9->data = nullptr;
layer_9->input_size = -1;
layer_9->output_size = -1;
layer_9->props = Multiplier{
    .num_dimensions = 1, .dimensions = {16}, .preceding = Layers::CONV2D};
layer_9->num_inputs = 1;
layer_9->num_outputs = 1;
auto layer_10 = std::dynamic_pointer_cast<Layer<Add>>(layers_[10]);
layer_10->activation = Activations::NONE;
layer_10->type = Layers::ADD;
layer_10->size = 16 * sizeof(float);
layer_10->input_size = -1;
layer_10->output_size = -1;
layer_10->data = nullptr;
layer_10->props =
    Add{.num_dimensions = 1, .dimensions = {16}, .preceding = Layers::CONV2D};
layer_10->num_inputs = 1;
layer_10->num_outputs = 1;

auto layer_11 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[11]);
layer_11->activation = Activations::NONE;
layer_11->type = Layers::CONV2D;
layer_11->size = 1 * 1 * 16 * 96 * sizeof(float);
layer_11->data = nullptr;
layer_11->input_size = -1;
layer_11->output_size = -1;
layer_11->num_inputs = 1;
layer_11->num_outputs = 1;
layer_11->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 16, 96},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_12 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[12]);
layer_12->activation = Activations::NONE;
layer_12->type = Layers::MULTIPLIER;
layer_12->size = 96 * sizeof(float);
layer_12->data = nullptr;
layer_12->input_size = -1;
layer_12->output_size = -1;
layer_12->props = Multiplier{
    .num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_12->num_inputs = 1;
layer_12->num_outputs = 1;
auto layer_13 = std::dynamic_pointer_cast<Layer<Add>>(layers_[13]);
layer_13->activation = Activations::NONE;
layer_13->type = Layers::ADD;
layer_13->size = 96 * sizeof(float);
layer_13->input_size = -1;
layer_13->output_size = -1;
layer_13->data = nullptr;
layer_13->props =
    Add{.num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_13->num_inputs = 1;
layer_13->num_outputs = 1;

auto layer_14 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[14]);
layer_14->activation = Activations::RELU;
layer_14->type = Layers::ACTIVATION;
layer_14->size = 0;
layer_14->data = nullptr;
layer_14->input_size = -1;
layer_14->output_size = -1;
layer_14->props = Mapper{.maxvalue = 6.0};
layer_14->num_inputs = 1;
layer_14->num_outputs = 1;

auto layer_15 = std::dynamic_pointer_cast<Layer<Padding>>(layers_[15]);
layer_15->activation = Activations::NONE;
layer_15->type = Layers::PADDING;
layer_15->size = 0 * sizeof(float);
layer_15->input_size = -1;
layer_15->output_size = -1;
layer_15->data = nullptr;
layer_15->props = Padding{.num_dimensions = 2, .padding = {0, 1, 0, 1}};
layer_15->num_inputs = 1;
layer_15->num_outputs = 1;

auto layer_16 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[16]);
layer_16->activation = Activations::NONE;
layer_16->type = Layers::DEPTHWISE_CONV2D;
layer_16->size = 3 * 3 * 96 * 1 * sizeof(float);
layer_16->data = nullptr;
layer_16->input_size = -1;
layer_16->output_size = -1;
layer_16->num_inputs = 1;
layer_16->num_outputs = 1;
layer_16->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 96, 1},
                              .strides = {2, 2, 1, 1},
                              .padding = {0, 0, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_17 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[17]);
layer_17->activation = Activations::NONE;
layer_17->type = Layers::MULTIPLIER;
layer_17->size = 96 * sizeof(float);
layer_17->data = nullptr;
layer_17->input_size = -1;
layer_17->output_size = -1;
layer_17->props = Multiplier{
    .num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_17->num_inputs = 1;
layer_17->num_outputs = 1;
auto layer_18 = std::dynamic_pointer_cast<Layer<Add>>(layers_[18]);
layer_18->activation = Activations::NONE;
layer_18->type = Layers::ADD;
layer_18->size = 96 * sizeof(float);
layer_18->input_size = -1;
layer_18->output_size = -1;
layer_18->data = nullptr;
layer_18->props =
    Add{.num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_18->num_inputs = 1;
layer_18->num_outputs = 1;

auto layer_19 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[19]);
layer_19->activation = Activations::RELU;
layer_19->type = Layers::ACTIVATION;
layer_19->size = 0;
layer_19->data = nullptr;
layer_19->input_size = -1;
layer_19->output_size = -1;
layer_19->props = Mapper{.maxvalue = 6.0};
layer_19->num_inputs = 1;
layer_19->num_outputs = 1;

auto layer_20 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[20]);
layer_20->activation = Activations::NONE;
layer_20->type = Layers::CONV2D;
layer_20->size = 1 * 1 * 96 * 24 * sizeof(float);
layer_20->data = nullptr;
layer_20->input_size = -1;
layer_20->output_size = -1;
layer_20->num_inputs = 1;
layer_20->num_outputs = 1;
layer_20->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 96, 24},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_21 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[21]);
layer_21->activation = Activations::NONE;
layer_21->type = Layers::MULTIPLIER;
layer_21->size = 24 * sizeof(float);
layer_21->data = nullptr;
layer_21->input_size = -1;
layer_21->output_size = -1;
layer_21->props = Multiplier{
    .num_dimensions = 1, .dimensions = {24}, .preceding = Layers::CONV2D};
layer_21->num_inputs = 1;
layer_21->num_outputs = 1;
auto layer_22 = std::dynamic_pointer_cast<Layer<Add>>(layers_[22]);
layer_22->activation = Activations::NONE;
layer_22->type = Layers::ADD;
layer_22->size = 24 * sizeof(float);
layer_22->input_size = -1;
layer_22->output_size = -1;
layer_22->data = nullptr;
layer_22->props =
    Add{.num_dimensions = 1, .dimensions = {24}, .preceding = Layers::CONV2D};
layer_22->num_inputs = 1;
layer_22->num_outputs = 1;

auto layer_23 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[23]);
layer_23->activation = Activations::NONE;
layer_23->type = Layers::CONV2D;
layer_23->size = 1 * 1 * 24 * 144 * sizeof(float);
layer_23->data = nullptr;
layer_23->input_size = -1;
layer_23->output_size = -1;
layer_23->num_inputs = 1;
layer_23->num_outputs = 1;
layer_23->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 24, 144},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_24 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[24]);
layer_24->activation = Activations::NONE;
layer_24->type = Layers::MULTIPLIER;
layer_24->size = 144 * sizeof(float);
layer_24->data = nullptr;
layer_24->input_size = -1;
layer_24->output_size = -1;
layer_24->props = Multiplier{
    .num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_24->num_inputs = 1;
layer_24->num_outputs = 1;
auto layer_25 = std::dynamic_pointer_cast<Layer<Add>>(layers_[25]);
layer_25->activation = Activations::NONE;
layer_25->type = Layers::ADD;
layer_25->size = 144 * sizeof(float);
layer_25->input_size = -1;
layer_25->output_size = -1;
layer_25->data = nullptr;
layer_25->props =
    Add{.num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_25->num_inputs = 1;
layer_25->num_outputs = 1;

auto layer_26 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[26]);
layer_26->activation = Activations::RELU;
layer_26->type = Layers::ACTIVATION;
layer_26->size = 0;
layer_26->data = nullptr;
layer_26->input_size = -1;
layer_26->output_size = -1;
layer_26->props = Mapper{.maxvalue = 6.0};
layer_26->num_inputs = 1;
layer_26->num_outputs = 1;

auto layer_27 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[27]);
layer_27->activation = Activations::NONE;
layer_27->type = Layers::DEPTHWISE_CONV2D;
layer_27->size = 3 * 3 * 144 * 1 * sizeof(float);
layer_27->data = nullptr;
layer_27->input_size = -1;
layer_27->output_size = -1;
layer_27->num_inputs = 1;
layer_27->num_outputs = 1;
layer_27->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 144, 1},
                              .strides = {1, 1, 1, 1},
                              .padding = {1, 1, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_28 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[28]);
layer_28->activation = Activations::NONE;
layer_28->type = Layers::MULTIPLIER;
layer_28->size = 144 * sizeof(float);
layer_28->data = nullptr;
layer_28->input_size = -1;
layer_28->output_size = -1;
layer_28->props = Multiplier{
    .num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_28->num_inputs = 1;
layer_28->num_outputs = 1;
auto layer_29 = std::dynamic_pointer_cast<Layer<Add>>(layers_[29]);
layer_29->activation = Activations::NONE;
layer_29->type = Layers::ADD;
layer_29->size = 144 * sizeof(float);
layer_29->input_size = -1;
layer_29->output_size = -1;
layer_29->data = nullptr;
layer_29->props =
    Add{.num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_29->num_inputs = 1;
layer_29->num_outputs = 1;

auto layer_30 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[30]);
layer_30->activation = Activations::RELU;
layer_30->type = Layers::ACTIVATION;
layer_30->size = 0;
layer_30->data = nullptr;
layer_30->input_size = -1;
layer_30->output_size = -1;
layer_30->props = Mapper{.maxvalue = 6.0};
layer_30->num_inputs = 1;
layer_30->num_outputs = 1;

auto layer_31 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[31]);
layer_31->activation = Activations::NONE;
layer_31->type = Layers::CONV2D;
layer_31->size = 1 * 1 * 144 * 24 * sizeof(float);
layer_31->data = nullptr;
layer_31->input_size = -1;
layer_31->output_size = -1;
layer_31->num_inputs = 1;
layer_31->num_outputs = 1;
layer_31->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 144, 24},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_32 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[32]);
layer_32->activation = Activations::NONE;
layer_32->type = Layers::MULTIPLIER;
layer_32->size = 24 * sizeof(float);
layer_32->data = nullptr;
layer_32->input_size = -1;
layer_32->output_size = -1;
layer_32->props = Multiplier{
    .num_dimensions = 1, .dimensions = {24}, .preceding = Layers::CONV2D};
layer_32->num_inputs = 1;
layer_32->num_outputs = 1;
auto layer_33 = std::dynamic_pointer_cast<Layer<Add>>(layers_[33]);
layer_33->activation = Activations::NONE;
layer_33->type = Layers::ADD;
layer_33->size = 24 * sizeof(float);
layer_33->input_size = -1;
layer_33->output_size = -1;
layer_33->data = nullptr;
layer_33->props =
    Add{.num_dimensions = 1, .dimensions = {24}, .preceding = Layers::CONV2D};
layer_33->num_inputs = 1;
layer_33->num_outputs = 1;

auto layer_34 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[34]);
layer_34->activation = Activations::NONE;
layer_34->type = Layers::LAYERWISE;
layer_34->size = 0;
layer_34->input_size = -1;
layer_34->output_size = -1;
layer_34->data = nullptr;
layer_34->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_34->num_inputs = 2;
layer_34->num_outputs = 1;

auto layer_35 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[35]);
layer_35->activation = Activations::NONE;
layer_35->type = Layers::CONV2D;
layer_35->size = 1 * 1 * 24 * 144 * sizeof(float);
layer_35->data = nullptr;
layer_35->input_size = -1;
layer_35->output_size = -1;
layer_35->num_inputs = 1;
layer_35->num_outputs = 1;
layer_35->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 24, 144},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_36 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[36]);
layer_36->activation = Activations::NONE;
layer_36->type = Layers::MULTIPLIER;
layer_36->size = 144 * sizeof(float);
layer_36->data = nullptr;
layer_36->input_size = -1;
layer_36->output_size = -1;
layer_36->props = Multiplier{
    .num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_36->num_inputs = 1;
layer_36->num_outputs = 1;
auto layer_37 = std::dynamic_pointer_cast<Layer<Add>>(layers_[37]);
layer_37->activation = Activations::NONE;
layer_37->type = Layers::ADD;
layer_37->size = 144 * sizeof(float);
layer_37->input_size = -1;
layer_37->output_size = -1;
layer_37->data = nullptr;
layer_37->props =
    Add{.num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_37->num_inputs = 1;
layer_37->num_outputs = 1;

auto layer_38 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[38]);
layer_38->activation = Activations::RELU;
layer_38->type = Layers::ACTIVATION;
layer_38->size = 0;
layer_38->data = nullptr;
layer_38->input_size = -1;
layer_38->output_size = -1;
layer_38->props = Mapper{.maxvalue = 6.0};
layer_38->num_inputs = 1;
layer_38->num_outputs = 1;

auto layer_39 = std::dynamic_pointer_cast<Layer<Padding>>(layers_[39]);
layer_39->activation = Activations::NONE;
layer_39->type = Layers::PADDING;
layer_39->size = 0 * sizeof(float);
layer_39->input_size = -1;
layer_39->output_size = -1;
layer_39->data = nullptr;
layer_39->props = Padding{.num_dimensions = 2, .padding = {0, 1, 0, 1}};
layer_39->num_inputs = 1;
layer_39->num_outputs = 1;

auto layer_40 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[40]);
layer_40->activation = Activations::NONE;
layer_40->type = Layers::DEPTHWISE_CONV2D;
layer_40->size = 3 * 3 * 144 * 1 * sizeof(float);
layer_40->data = nullptr;
layer_40->input_size = -1;
layer_40->output_size = -1;
layer_40->num_inputs = 1;
layer_40->num_outputs = 1;
layer_40->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 144, 1},
                              .strides = {2, 2, 1, 1},
                              .padding = {0, 0, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_41 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[41]);
layer_41->activation = Activations::NONE;
layer_41->type = Layers::MULTIPLIER;
layer_41->size = 144 * sizeof(float);
layer_41->data = nullptr;
layer_41->input_size = -1;
layer_41->output_size = -1;
layer_41->props = Multiplier{
    .num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_41->num_inputs = 1;
layer_41->num_outputs = 1;
auto layer_42 = std::dynamic_pointer_cast<Layer<Add>>(layers_[42]);
layer_42->activation = Activations::NONE;
layer_42->type = Layers::ADD;
layer_42->size = 144 * sizeof(float);
layer_42->input_size = -1;
layer_42->output_size = -1;
layer_42->data = nullptr;
layer_42->props =
    Add{.num_dimensions = 1, .dimensions = {144}, .preceding = Layers::CONV2D};
layer_42->num_inputs = 1;
layer_42->num_outputs = 1;

auto layer_43 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[43]);
layer_43->activation = Activations::RELU;
layer_43->type = Layers::ACTIVATION;
layer_43->size = 0;
layer_43->data = nullptr;
layer_43->input_size = -1;
layer_43->output_size = -1;
layer_43->props = Mapper{.maxvalue = 6.0};
layer_43->num_inputs = 1;
layer_43->num_outputs = 1;

auto layer_44 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[44]);
layer_44->activation = Activations::NONE;
layer_44->type = Layers::CONV2D;
layer_44->size = 1 * 1 * 144 * 32 * sizeof(float);
layer_44->data = nullptr;
layer_44->input_size = -1;
layer_44->output_size = -1;
layer_44->num_inputs = 1;
layer_44->num_outputs = 1;
layer_44->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 144, 32},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_45 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[45]);
layer_45->activation = Activations::NONE;
layer_45->type = Layers::MULTIPLIER;
layer_45->size = 32 * sizeof(float);
layer_45->data = nullptr;
layer_45->input_size = -1;
layer_45->output_size = -1;
layer_45->props = Multiplier{
    .num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_45->num_inputs = 1;
layer_45->num_outputs = 1;
auto layer_46 = std::dynamic_pointer_cast<Layer<Add>>(layers_[46]);
layer_46->activation = Activations::NONE;
layer_46->type = Layers::ADD;
layer_46->size = 32 * sizeof(float);
layer_46->input_size = -1;
layer_46->output_size = -1;
layer_46->data = nullptr;
layer_46->props =
    Add{.num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_46->num_inputs = 1;
layer_46->num_outputs = 1;

auto layer_47 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[47]);
layer_47->activation = Activations::NONE;
layer_47->type = Layers::CONV2D;
layer_47->size = 1 * 1 * 32 * 192 * sizeof(float);
layer_47->data = nullptr;
layer_47->input_size = -1;
layer_47->output_size = -1;
layer_47->num_inputs = 1;
layer_47->num_outputs = 1;
layer_47->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 32, 192},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_48 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[48]);
layer_48->activation = Activations::NONE;
layer_48->type = Layers::MULTIPLIER;
layer_48->size = 192 * sizeof(float);
layer_48->data = nullptr;
layer_48->input_size = -1;
layer_48->output_size = -1;
layer_48->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_48->num_inputs = 1;
layer_48->num_outputs = 1;
auto layer_49 = std::dynamic_pointer_cast<Layer<Add>>(layers_[49]);
layer_49->activation = Activations::NONE;
layer_49->type = Layers::ADD;
layer_49->size = 192 * sizeof(float);
layer_49->input_size = -1;
layer_49->output_size = -1;
layer_49->data = nullptr;
layer_49->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_49->num_inputs = 1;
layer_49->num_outputs = 1;

auto layer_50 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[50]);
layer_50->activation = Activations::RELU;
layer_50->type = Layers::ACTIVATION;
layer_50->size = 0;
layer_50->data = nullptr;
layer_50->input_size = -1;
layer_50->output_size = -1;
layer_50->props = Mapper{.maxvalue = 6.0};
layer_50->num_inputs = 1;
layer_50->num_outputs = 1;

auto layer_51 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[51]);
layer_51->activation = Activations::NONE;
layer_51->type = Layers::DEPTHWISE_CONV2D;
layer_51->size = 3 * 3 * 192 * 1 * sizeof(float);
layer_51->data = nullptr;
layer_51->input_size = -1;
layer_51->output_size = -1;
layer_51->num_inputs = 1;
layer_51->num_outputs = 1;
layer_51->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 192, 1},
                              .strides = {1, 1, 1, 1},
                              .padding = {1, 1, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_52 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[52]);
layer_52->activation = Activations::NONE;
layer_52->type = Layers::MULTIPLIER;
layer_52->size = 192 * sizeof(float);
layer_52->data = nullptr;
layer_52->input_size = -1;
layer_52->output_size = -1;
layer_52->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_52->num_inputs = 1;
layer_52->num_outputs = 1;
auto layer_53 = std::dynamic_pointer_cast<Layer<Add>>(layers_[53]);
layer_53->activation = Activations::NONE;
layer_53->type = Layers::ADD;
layer_53->size = 192 * sizeof(float);
layer_53->input_size = -1;
layer_53->output_size = -1;
layer_53->data = nullptr;
layer_53->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_53->num_inputs = 1;
layer_53->num_outputs = 1;

auto layer_54 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[54]);
layer_54->activation = Activations::RELU;
layer_54->type = Layers::ACTIVATION;
layer_54->size = 0;
layer_54->data = nullptr;
layer_54->input_size = -1;
layer_54->output_size = -1;
layer_54->props = Mapper{.maxvalue = 6.0};
layer_54->num_inputs = 1;
layer_54->num_outputs = 1;

auto layer_55 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[55]);
layer_55->activation = Activations::NONE;
layer_55->type = Layers::CONV2D;
layer_55->size = 1 * 1 * 192 * 32 * sizeof(float);
layer_55->data = nullptr;
layer_55->input_size = -1;
layer_55->output_size = -1;
layer_55->num_inputs = 1;
layer_55->num_outputs = 1;
layer_55->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 192, 32},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_56 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[56]);
layer_56->activation = Activations::NONE;
layer_56->type = Layers::MULTIPLIER;
layer_56->size = 32 * sizeof(float);
layer_56->data = nullptr;
layer_56->input_size = -1;
layer_56->output_size = -1;
layer_56->props = Multiplier{
    .num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_56->num_inputs = 1;
layer_56->num_outputs = 1;
auto layer_57 = std::dynamic_pointer_cast<Layer<Add>>(layers_[57]);
layer_57->activation = Activations::NONE;
layer_57->type = Layers::ADD;
layer_57->size = 32 * sizeof(float);
layer_57->input_size = -1;
layer_57->output_size = -1;
layer_57->data = nullptr;
layer_57->props =
    Add{.num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_57->num_inputs = 1;
layer_57->num_outputs = 1;

auto layer_58 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[58]);
layer_58->activation = Activations::NONE;
layer_58->type = Layers::LAYERWISE;
layer_58->size = 0;
layer_58->input_size = -1;
layer_58->output_size = -1;
layer_58->data = nullptr;
layer_58->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_58->num_inputs = 2;
layer_58->num_outputs = 1;

auto layer_59 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[59]);
layer_59->activation = Activations::NONE;
layer_59->type = Layers::CONV2D;
layer_59->size = 1 * 1 * 32 * 192 * sizeof(float);
layer_59->data = nullptr;
layer_59->input_size = -1;
layer_59->output_size = -1;
layer_59->num_inputs = 1;
layer_59->num_outputs = 1;
layer_59->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 32, 192},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_60 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[60]);
layer_60->activation = Activations::NONE;
layer_60->type = Layers::MULTIPLIER;
layer_60->size = 192 * sizeof(float);
layer_60->data = nullptr;
layer_60->input_size = -1;
layer_60->output_size = -1;
layer_60->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_60->num_inputs = 1;
layer_60->num_outputs = 1;
auto layer_61 = std::dynamic_pointer_cast<Layer<Add>>(layers_[61]);
layer_61->activation = Activations::NONE;
layer_61->type = Layers::ADD;
layer_61->size = 192 * sizeof(float);
layer_61->input_size = -1;
layer_61->output_size = -1;
layer_61->data = nullptr;
layer_61->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_61->num_inputs = 1;
layer_61->num_outputs = 1;

auto layer_62 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[62]);
layer_62->activation = Activations::RELU;
layer_62->type = Layers::ACTIVATION;
layer_62->size = 0;
layer_62->data = nullptr;
layer_62->input_size = -1;
layer_62->output_size = -1;
layer_62->props = Mapper{.maxvalue = 6.0};
layer_62->num_inputs = 1;
layer_62->num_outputs = 1;

auto layer_63 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[63]);
layer_63->activation = Activations::NONE;
layer_63->type = Layers::DEPTHWISE_CONV2D;
layer_63->size = 3 * 3 * 192 * 1 * sizeof(float);
layer_63->data = nullptr;
layer_63->input_size = -1;
layer_63->output_size = -1;
layer_63->num_inputs = 1;
layer_63->num_outputs = 1;
layer_63->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 192, 1},
                              .strides = {1, 1, 1, 1},
                              .padding = {1, 1, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_64 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[64]);
layer_64->activation = Activations::NONE;
layer_64->type = Layers::MULTIPLIER;
layer_64->size = 192 * sizeof(float);
layer_64->data = nullptr;
layer_64->input_size = -1;
layer_64->output_size = -1;
layer_64->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_64->num_inputs = 1;
layer_64->num_outputs = 1;
auto layer_65 = std::dynamic_pointer_cast<Layer<Add>>(layers_[65]);
layer_65->activation = Activations::NONE;
layer_65->type = Layers::ADD;
layer_65->size = 192 * sizeof(float);
layer_65->input_size = -1;
layer_65->output_size = -1;
layer_65->data = nullptr;
layer_65->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_65->num_inputs = 1;
layer_65->num_outputs = 1;

auto layer_66 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[66]);
layer_66->activation = Activations::RELU;
layer_66->type = Layers::ACTIVATION;
layer_66->size = 0;
layer_66->data = nullptr;
layer_66->input_size = -1;
layer_66->output_size = -1;
layer_66->props = Mapper{.maxvalue = 6.0};
layer_66->num_inputs = 1;
layer_66->num_outputs = 1;

auto layer_67 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[67]);
layer_67->activation = Activations::NONE;
layer_67->type = Layers::CONV2D;
layer_67->size = 1 * 1 * 192 * 32 * sizeof(float);
layer_67->data = nullptr;
layer_67->input_size = -1;
layer_67->output_size = -1;
layer_67->num_inputs = 1;
layer_67->num_outputs = 1;
layer_67->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 192, 32},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_68 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[68]);
layer_68->activation = Activations::NONE;
layer_68->type = Layers::MULTIPLIER;
layer_68->size = 32 * sizeof(float);
layer_68->data = nullptr;
layer_68->input_size = -1;
layer_68->output_size = -1;
layer_68->props = Multiplier{
    .num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_68->num_inputs = 1;
layer_68->num_outputs = 1;
auto layer_69 = std::dynamic_pointer_cast<Layer<Add>>(layers_[69]);
layer_69->activation = Activations::NONE;
layer_69->type = Layers::ADD;
layer_69->size = 32 * sizeof(float);
layer_69->input_size = -1;
layer_69->output_size = -1;
layer_69->data = nullptr;
layer_69->props =
    Add{.num_dimensions = 1, .dimensions = {32}, .preceding = Layers::CONV2D};
layer_69->num_inputs = 1;
layer_69->num_outputs = 1;

auto layer_70 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[70]);
layer_70->activation = Activations::NONE;
layer_70->type = Layers::LAYERWISE;
layer_70->size = 0;
layer_70->input_size = -1;
layer_70->output_size = -1;
layer_70->data = nullptr;
layer_70->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_70->num_inputs = 2;
layer_70->num_outputs = 1;

auto layer_71 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[71]);
layer_71->activation = Activations::NONE;
layer_71->type = Layers::CONV2D;
layer_71->size = 1 * 1 * 32 * 192 * sizeof(float);
layer_71->data = nullptr;
layer_71->input_size = -1;
layer_71->output_size = -1;
layer_71->num_inputs = 1;
layer_71->num_outputs = 1;
layer_71->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 32, 192},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_72 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[72]);
layer_72->activation = Activations::NONE;
layer_72->type = Layers::MULTIPLIER;
layer_72->size = 192 * sizeof(float);
layer_72->data = nullptr;
layer_72->input_size = -1;
layer_72->output_size = -1;
layer_72->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_72->num_inputs = 1;
layer_72->num_outputs = 1;
auto layer_73 = std::dynamic_pointer_cast<Layer<Add>>(layers_[73]);
layer_73->activation = Activations::NONE;
layer_73->type = Layers::ADD;
layer_73->size = 192 * sizeof(float);
layer_73->input_size = -1;
layer_73->output_size = -1;
layer_73->data = nullptr;
layer_73->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_73->num_inputs = 1;
layer_73->num_outputs = 1;

auto layer_74 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[74]);
layer_74->activation = Activations::RELU;
layer_74->type = Layers::ACTIVATION;
layer_74->size = 0;
layer_74->data = nullptr;
layer_74->input_size = -1;
layer_74->output_size = -1;
layer_74->props = Mapper{.maxvalue = 6.0};
layer_74->num_inputs = 1;
layer_74->num_outputs = 1;

auto layer_75 = std::dynamic_pointer_cast<Layer<Padding>>(layers_[75]);
layer_75->activation = Activations::NONE;
layer_75->type = Layers::PADDING;
layer_75->size = 0 * sizeof(float);
layer_75->input_size = -1;
layer_75->output_size = -1;
layer_75->data = nullptr;
layer_75->props = Padding{.num_dimensions = 2, .padding = {0, 1, 0, 1}};
layer_75->num_inputs = 1;
layer_75->num_outputs = 1;

auto layer_76 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[76]);
layer_76->activation = Activations::NONE;
layer_76->type = Layers::DEPTHWISE_CONV2D;
layer_76->size = 3 * 3 * 192 * 1 * sizeof(float);
layer_76->data = nullptr;
layer_76->input_size = -1;
layer_76->output_size = -1;
layer_76->num_inputs = 1;
layer_76->num_outputs = 1;
layer_76->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 192, 1},
                              .strides = {2, 2, 1, 1},
                              .padding = {0, 0, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_77 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[77]);
layer_77->activation = Activations::NONE;
layer_77->type = Layers::MULTIPLIER;
layer_77->size = 192 * sizeof(float);
layer_77->data = nullptr;
layer_77->input_size = -1;
layer_77->output_size = -1;
layer_77->props = Multiplier{
    .num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_77->num_inputs = 1;
layer_77->num_outputs = 1;
auto layer_78 = std::dynamic_pointer_cast<Layer<Add>>(layers_[78]);
layer_78->activation = Activations::NONE;
layer_78->type = Layers::ADD;
layer_78->size = 192 * sizeof(float);
layer_78->input_size = -1;
layer_78->output_size = -1;
layer_78->data = nullptr;
layer_78->props =
    Add{.num_dimensions = 1, .dimensions = {192}, .preceding = Layers::CONV2D};
layer_78->num_inputs = 1;
layer_78->num_outputs = 1;

auto layer_79 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[79]);
layer_79->activation = Activations::RELU;
layer_79->type = Layers::ACTIVATION;
layer_79->size = 0;
layer_79->data = nullptr;
layer_79->input_size = -1;
layer_79->output_size = -1;
layer_79->props = Mapper{.maxvalue = 6.0};
layer_79->num_inputs = 1;
layer_79->num_outputs = 1;

auto layer_80 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[80]);
layer_80->activation = Activations::NONE;
layer_80->type = Layers::CONV2D;
layer_80->size = 1 * 1 * 192 * 64 * sizeof(float);
layer_80->data = nullptr;
layer_80->input_size = -1;
layer_80->output_size = -1;
layer_80->num_inputs = 1;
layer_80->num_outputs = 1;
layer_80->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 192, 64},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_81 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[81]);
layer_81->activation = Activations::NONE;
layer_81->type = Layers::MULTIPLIER;
layer_81->size = 64 * sizeof(float);
layer_81->data = nullptr;
layer_81->input_size = -1;
layer_81->output_size = -1;
layer_81->props = Multiplier{
    .num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_81->num_inputs = 1;
layer_81->num_outputs = 1;
auto layer_82 = std::dynamic_pointer_cast<Layer<Add>>(layers_[82]);
layer_82->activation = Activations::NONE;
layer_82->type = Layers::ADD;
layer_82->size = 64 * sizeof(float);
layer_82->input_size = -1;
layer_82->output_size = -1;
layer_82->data = nullptr;
layer_82->props =
    Add{.num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_82->num_inputs = 1;
layer_82->num_outputs = 1;

auto layer_83 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[83]);
layer_83->activation = Activations::NONE;
layer_83->type = Layers::CONV2D;
layer_83->size = 1 * 1 * 64 * 384 * sizeof(float);
layer_83->data = nullptr;
layer_83->input_size = -1;
layer_83->output_size = -1;
layer_83->num_inputs = 1;
layer_83->num_outputs = 1;
layer_83->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 64, 384},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_84 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[84]);
layer_84->activation = Activations::NONE;
layer_84->type = Layers::MULTIPLIER;
layer_84->size = 384 * sizeof(float);
layer_84->data = nullptr;
layer_84->input_size = -1;
layer_84->output_size = -1;
layer_84->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_84->num_inputs = 1;
layer_84->num_outputs = 1;
auto layer_85 = std::dynamic_pointer_cast<Layer<Add>>(layers_[85]);
layer_85->activation = Activations::NONE;
layer_85->type = Layers::ADD;
layer_85->size = 384 * sizeof(float);
layer_85->input_size = -1;
layer_85->output_size = -1;
layer_85->data = nullptr;
layer_85->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_85->num_inputs = 1;
layer_85->num_outputs = 1;

auto layer_86 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[86]);
layer_86->activation = Activations::RELU;
layer_86->type = Layers::ACTIVATION;
layer_86->size = 0;
layer_86->data = nullptr;
layer_86->input_size = -1;
layer_86->output_size = -1;
layer_86->props = Mapper{.maxvalue = 6.0};
layer_86->num_inputs = 1;
layer_86->num_outputs = 1;

auto layer_87 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[87]);
layer_87->activation = Activations::NONE;
layer_87->type = Layers::DEPTHWISE_CONV2D;
layer_87->size = 3 * 3 * 384 * 1 * sizeof(float);
layer_87->data = nullptr;
layer_87->input_size = -1;
layer_87->output_size = -1;
layer_87->num_inputs = 1;
layer_87->num_outputs = 1;
layer_87->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 384, 1},
                              .strides = {1, 1, 1, 1},
                              .padding = {1, 1, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_88 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[88]);
layer_88->activation = Activations::NONE;
layer_88->type = Layers::MULTIPLIER;
layer_88->size = 384 * sizeof(float);
layer_88->data = nullptr;
layer_88->input_size = -1;
layer_88->output_size = -1;
layer_88->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_88->num_inputs = 1;
layer_88->num_outputs = 1;
auto layer_89 = std::dynamic_pointer_cast<Layer<Add>>(layers_[89]);
layer_89->activation = Activations::NONE;
layer_89->type = Layers::ADD;
layer_89->size = 384 * sizeof(float);
layer_89->input_size = -1;
layer_89->output_size = -1;
layer_89->data = nullptr;
layer_89->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_89->num_inputs = 1;
layer_89->num_outputs = 1;

auto layer_90 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[90]);
layer_90->activation = Activations::RELU;
layer_90->type = Layers::ACTIVATION;
layer_90->size = 0;
layer_90->data = nullptr;
layer_90->input_size = -1;
layer_90->output_size = -1;
layer_90->props = Mapper{.maxvalue = 6.0};
layer_90->num_inputs = 1;
layer_90->num_outputs = 1;

auto layer_91 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[91]);
layer_91->activation = Activations::NONE;
layer_91->type = Layers::CONV2D;
layer_91->size = 1 * 1 * 384 * 64 * sizeof(float);
layer_91->data = nullptr;
layer_91->input_size = -1;
layer_91->output_size = -1;
layer_91->num_inputs = 1;
layer_91->num_outputs = 1;
layer_91->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 384, 64},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_92 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[92]);
layer_92->activation = Activations::NONE;
layer_92->type = Layers::MULTIPLIER;
layer_92->size = 64 * sizeof(float);
layer_92->data = nullptr;
layer_92->input_size = -1;
layer_92->output_size = -1;
layer_92->props = Multiplier{
    .num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_92->num_inputs = 1;
layer_92->num_outputs = 1;
auto layer_93 = std::dynamic_pointer_cast<Layer<Add>>(layers_[93]);
layer_93->activation = Activations::NONE;
layer_93->type = Layers::ADD;
layer_93->size = 64 * sizeof(float);
layer_93->input_size = -1;
layer_93->output_size = -1;
layer_93->data = nullptr;
layer_93->props =
    Add{.num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_93->num_inputs = 1;
layer_93->num_outputs = 1;

auto layer_94 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[94]);
layer_94->activation = Activations::NONE;
layer_94->type = Layers::LAYERWISE;
layer_94->size = 0;
layer_94->input_size = -1;
layer_94->output_size = -1;
layer_94->data = nullptr;
layer_94->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_94->num_inputs = 2;
layer_94->num_outputs = 1;

auto layer_95 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[95]);
layer_95->activation = Activations::NONE;
layer_95->type = Layers::CONV2D;
layer_95->size = 1 * 1 * 64 * 384 * sizeof(float);
layer_95->data = nullptr;
layer_95->input_size = -1;
layer_95->output_size = -1;
layer_95->num_inputs = 1;
layer_95->num_outputs = 1;
layer_95->props = Conv2D{.num_dimensions = 4,
                         .dimensions = {1, 1, 64, 384},
                         .strides = {1, 1, 1, 1},
                         .padding = {0, 0, 0, 0},
                         .dilatations = {1, 1, 1, 1}};

auto layer_96 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[96]);
layer_96->activation = Activations::NONE;
layer_96->type = Layers::MULTIPLIER;
layer_96->size = 384 * sizeof(float);
layer_96->data = nullptr;
layer_96->input_size = -1;
layer_96->output_size = -1;
layer_96->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_96->num_inputs = 1;
layer_96->num_outputs = 1;
auto layer_97 = std::dynamic_pointer_cast<Layer<Add>>(layers_[97]);
layer_97->activation = Activations::NONE;
layer_97->type = Layers::ADD;
layer_97->size = 384 * sizeof(float);
layer_97->input_size = -1;
layer_97->output_size = -1;
layer_97->data = nullptr;
layer_97->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_97->num_inputs = 1;
layer_97->num_outputs = 1;

auto layer_98 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[98]);
layer_98->activation = Activations::RELU;
layer_98->type = Layers::ACTIVATION;
layer_98->size = 0;
layer_98->data = nullptr;
layer_98->input_size = -1;
layer_98->output_size = -1;
layer_98->props = Mapper{.maxvalue = 6.0};
layer_98->num_inputs = 1;
layer_98->num_outputs = 1;

auto layer_99 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[99]);
layer_99->activation = Activations::NONE;
layer_99->type = Layers::DEPTHWISE_CONV2D;
layer_99->size = 3 * 3 * 384 * 1 * sizeof(float);
layer_99->data = nullptr;
layer_99->input_size = -1;
layer_99->output_size = -1;
layer_99->num_inputs = 1;
layer_99->num_outputs = 1;
layer_99->props = DepthConv2D{.num_dimensions = 3,
                              .dimensions = {3, 3, 384, 1},
                              .strides = {1, 1, 1, 1},
                              .padding = {1, 1, 0, 0},
                              .dilatations = {1, 1, 1, 1}};

auto layer_100 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[100]);
layer_100->activation = Activations::NONE;
layer_100->type = Layers::MULTIPLIER;
layer_100->size = 384 * sizeof(float);
layer_100->data = nullptr;
layer_100->input_size = -1;
layer_100->output_size = -1;
layer_100->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_100->num_inputs = 1;
layer_100->num_outputs = 1;
auto layer_101 = std::dynamic_pointer_cast<Layer<Add>>(layers_[101]);
layer_101->activation = Activations::NONE;
layer_101->type = Layers::ADD;
layer_101->size = 384 * sizeof(float);
layer_101->input_size = -1;
layer_101->output_size = -1;
layer_101->data = nullptr;
layer_101->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_101->num_inputs = 1;
layer_101->num_outputs = 1;

auto layer_102 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[102]);
layer_102->activation = Activations::RELU;
layer_102->type = Layers::ACTIVATION;
layer_102->size = 0;
layer_102->data = nullptr;
layer_102->input_size = -1;
layer_102->output_size = -1;
layer_102->props = Mapper{.maxvalue = 6.0};
layer_102->num_inputs = 1;
layer_102->num_outputs = 1;

auto layer_103 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[103]);
layer_103->activation = Activations::NONE;
layer_103->type = Layers::CONV2D;
layer_103->size = 1 * 1 * 384 * 64 * sizeof(float);
layer_103->data = nullptr;
layer_103->input_size = -1;
layer_103->output_size = -1;
layer_103->num_inputs = 1;
layer_103->num_outputs = 1;
layer_103->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 384, 64},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_104 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[104]);
layer_104->activation = Activations::NONE;
layer_104->type = Layers::MULTIPLIER;
layer_104->size = 64 * sizeof(float);
layer_104->data = nullptr;
layer_104->input_size = -1;
layer_104->output_size = -1;
layer_104->props = Multiplier{
    .num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_104->num_inputs = 1;
layer_104->num_outputs = 1;
auto layer_105 = std::dynamic_pointer_cast<Layer<Add>>(layers_[105]);
layer_105->activation = Activations::NONE;
layer_105->type = Layers::ADD;
layer_105->size = 64 * sizeof(float);
layer_105->input_size = -1;
layer_105->output_size = -1;
layer_105->data = nullptr;
layer_105->props =
    Add{.num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_105->num_inputs = 1;
layer_105->num_outputs = 1;

auto layer_106 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[106]);
layer_106->activation = Activations::NONE;
layer_106->type = Layers::LAYERWISE;
layer_106->size = 0;
layer_106->input_size = -1;
layer_106->output_size = -1;
layer_106->data = nullptr;
layer_106->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_106->num_inputs = 2;
layer_106->num_outputs = 1;

auto layer_107 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[107]);
layer_107->activation = Activations::NONE;
layer_107->type = Layers::CONV2D;
layer_107->size = 1 * 1 * 64 * 384 * sizeof(float);
layer_107->data = nullptr;
layer_107->input_size = -1;
layer_107->output_size = -1;
layer_107->num_inputs = 1;
layer_107->num_outputs = 1;
layer_107->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 64, 384},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_108 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[108]);
layer_108->activation = Activations::NONE;
layer_108->type = Layers::MULTIPLIER;
layer_108->size = 384 * sizeof(float);
layer_108->data = nullptr;
layer_108->input_size = -1;
layer_108->output_size = -1;
layer_108->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_108->num_inputs = 1;
layer_108->num_outputs = 1;
auto layer_109 = std::dynamic_pointer_cast<Layer<Add>>(layers_[109]);
layer_109->activation = Activations::NONE;
layer_109->type = Layers::ADD;
layer_109->size = 384 * sizeof(float);
layer_109->input_size = -1;
layer_109->output_size = -1;
layer_109->data = nullptr;
layer_109->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_109->num_inputs = 1;
layer_109->num_outputs = 1;

auto layer_110 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[110]);
layer_110->activation = Activations::RELU;
layer_110->type = Layers::ACTIVATION;
layer_110->size = 0;
layer_110->data = nullptr;
layer_110->input_size = -1;
layer_110->output_size = -1;
layer_110->props = Mapper{.maxvalue = 6.0};
layer_110->num_inputs = 1;
layer_110->num_outputs = 1;

auto layer_111 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[111]);
layer_111->activation = Activations::NONE;
layer_111->type = Layers::DEPTHWISE_CONV2D;
layer_111->size = 3 * 3 * 384 * 1 * sizeof(float);
layer_111->data = nullptr;
layer_111->input_size = -1;
layer_111->output_size = -1;
layer_111->num_inputs = 1;
layer_111->num_outputs = 1;
layer_111->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 384, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_112 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[112]);
layer_112->activation = Activations::NONE;
layer_112->type = Layers::MULTIPLIER;
layer_112->size = 384 * sizeof(float);
layer_112->data = nullptr;
layer_112->input_size = -1;
layer_112->output_size = -1;
layer_112->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_112->num_inputs = 1;
layer_112->num_outputs = 1;
auto layer_113 = std::dynamic_pointer_cast<Layer<Add>>(layers_[113]);
layer_113->activation = Activations::NONE;
layer_113->type = Layers::ADD;
layer_113->size = 384 * sizeof(float);
layer_113->input_size = -1;
layer_113->output_size = -1;
layer_113->data = nullptr;
layer_113->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_113->num_inputs = 1;
layer_113->num_outputs = 1;

auto layer_114 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[114]);
layer_114->activation = Activations::RELU;
layer_114->type = Layers::ACTIVATION;
layer_114->size = 0;
layer_114->data = nullptr;
layer_114->input_size = -1;
layer_114->output_size = -1;
layer_114->props = Mapper{.maxvalue = 6.0};
layer_114->num_inputs = 1;
layer_114->num_outputs = 1;

auto layer_115 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[115]);
layer_115->activation = Activations::NONE;
layer_115->type = Layers::CONV2D;
layer_115->size = 1 * 1 * 384 * 64 * sizeof(float);
layer_115->data = nullptr;
layer_115->input_size = -1;
layer_115->output_size = -1;
layer_115->num_inputs = 1;
layer_115->num_outputs = 1;
layer_115->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 384, 64},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_116 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[116]);
layer_116->activation = Activations::NONE;
layer_116->type = Layers::MULTIPLIER;
layer_116->size = 64 * sizeof(float);
layer_116->data = nullptr;
layer_116->input_size = -1;
layer_116->output_size = -1;
layer_116->props = Multiplier{
    .num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_116->num_inputs = 1;
layer_116->num_outputs = 1;
auto layer_117 = std::dynamic_pointer_cast<Layer<Add>>(layers_[117]);
layer_117->activation = Activations::NONE;
layer_117->type = Layers::ADD;
layer_117->size = 64 * sizeof(float);
layer_117->input_size = -1;
layer_117->output_size = -1;
layer_117->data = nullptr;
layer_117->props =
    Add{.num_dimensions = 1, .dimensions = {64}, .preceding = Layers::CONV2D};
layer_117->num_inputs = 1;
layer_117->num_outputs = 1;

auto layer_118 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[118]);
layer_118->activation = Activations::NONE;
layer_118->type = Layers::LAYERWISE;
layer_118->size = 0;
layer_118->input_size = -1;
layer_118->output_size = -1;
layer_118->data = nullptr;
layer_118->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_118->num_inputs = 2;
layer_118->num_outputs = 1;

auto layer_119 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[119]);
layer_119->activation = Activations::NONE;
layer_119->type = Layers::CONV2D;
layer_119->size = 1 * 1 * 64 * 384 * sizeof(float);
layer_119->data = nullptr;
layer_119->input_size = -1;
layer_119->output_size = -1;
layer_119->num_inputs = 1;
layer_119->num_outputs = 1;
layer_119->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 64, 384},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_120 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[120]);
layer_120->activation = Activations::NONE;
layer_120->type = Layers::MULTIPLIER;
layer_120->size = 384 * sizeof(float);
layer_120->data = nullptr;
layer_120->input_size = -1;
layer_120->output_size = -1;
layer_120->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_120->num_inputs = 1;
layer_120->num_outputs = 1;
auto layer_121 = std::dynamic_pointer_cast<Layer<Add>>(layers_[121]);
layer_121->activation = Activations::NONE;
layer_121->type = Layers::ADD;
layer_121->size = 384 * sizeof(float);
layer_121->input_size = -1;
layer_121->output_size = -1;
layer_121->data = nullptr;
layer_121->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_121->num_inputs = 1;
layer_121->num_outputs = 1;

auto layer_122 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[122]);
layer_122->activation = Activations::RELU;
layer_122->type = Layers::ACTIVATION;
layer_122->size = 0;
layer_122->data = nullptr;
layer_122->input_size = -1;
layer_122->output_size = -1;
layer_122->props = Mapper{.maxvalue = 6.0};
layer_122->num_inputs = 1;
layer_122->num_outputs = 1;

auto layer_123 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[123]);
layer_123->activation = Activations::NONE;
layer_123->type = Layers::DEPTHWISE_CONV2D;
layer_123->size = 3 * 3 * 384 * 1 * sizeof(float);
layer_123->data = nullptr;
layer_123->input_size = -1;
layer_123->output_size = -1;
layer_123->num_inputs = 1;
layer_123->num_outputs = 1;
layer_123->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 384, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_124 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[124]);
layer_124->activation = Activations::NONE;
layer_124->type = Layers::MULTIPLIER;
layer_124->size = 384 * sizeof(float);
layer_124->data = nullptr;
layer_124->input_size = -1;
layer_124->output_size = -1;
layer_124->props = Multiplier{
    .num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_124->num_inputs = 1;
layer_124->num_outputs = 1;
auto layer_125 = std::dynamic_pointer_cast<Layer<Add>>(layers_[125]);
layer_125->activation = Activations::NONE;
layer_125->type = Layers::ADD;
layer_125->size = 384 * sizeof(float);
layer_125->input_size = -1;
layer_125->output_size = -1;
layer_125->data = nullptr;
layer_125->props =
    Add{.num_dimensions = 1, .dimensions = {384}, .preceding = Layers::CONV2D};
layer_125->num_inputs = 1;
layer_125->num_outputs = 1;

auto layer_126 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[126]);
layer_126->activation = Activations::RELU;
layer_126->type = Layers::ACTIVATION;
layer_126->size = 0;
layer_126->data = nullptr;
layer_126->input_size = -1;
layer_126->output_size = -1;
layer_126->props = Mapper{.maxvalue = 6.0};
layer_126->num_inputs = 1;
layer_126->num_outputs = 1;

auto layer_127 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[127]);
layer_127->activation = Activations::NONE;
layer_127->type = Layers::CONV2D;
layer_127->size = 1 * 1 * 384 * 96 * sizeof(float);
layer_127->data = nullptr;
layer_127->input_size = -1;
layer_127->output_size = -1;
layer_127->num_inputs = 1;
layer_127->num_outputs = 1;
layer_127->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 384, 96},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_128 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[128]);
layer_128->activation = Activations::NONE;
layer_128->type = Layers::MULTIPLIER;
layer_128->size = 96 * sizeof(float);
layer_128->data = nullptr;
layer_128->input_size = -1;
layer_128->output_size = -1;
layer_128->props = Multiplier{
    .num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_128->num_inputs = 1;
layer_128->num_outputs = 1;
auto layer_129 = std::dynamic_pointer_cast<Layer<Add>>(layers_[129]);
layer_129->activation = Activations::NONE;
layer_129->type = Layers::ADD;
layer_129->size = 96 * sizeof(float);
layer_129->input_size = -1;
layer_129->output_size = -1;
layer_129->data = nullptr;
layer_129->props =
    Add{.num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_129->num_inputs = 1;
layer_129->num_outputs = 1;

auto layer_130 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[130]);
layer_130->activation = Activations::NONE;
layer_130->type = Layers::CONV2D;
layer_130->size = 1 * 1 * 96 * 576 * sizeof(float);
layer_130->data = nullptr;
layer_130->input_size = -1;
layer_130->output_size = -1;
layer_130->num_inputs = 1;
layer_130->num_outputs = 1;
layer_130->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 96, 576},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_131 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[131]);
layer_131->activation = Activations::NONE;
layer_131->type = Layers::MULTIPLIER;
layer_131->size = 576 * sizeof(float);
layer_131->data = nullptr;
layer_131->input_size = -1;
layer_131->output_size = -1;
layer_131->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_131->num_inputs = 1;
layer_131->num_outputs = 1;
auto layer_132 = std::dynamic_pointer_cast<Layer<Add>>(layers_[132]);
layer_132->activation = Activations::NONE;
layer_132->type = Layers::ADD;
layer_132->size = 576 * sizeof(float);
layer_132->input_size = -1;
layer_132->output_size = -1;
layer_132->data = nullptr;
layer_132->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_132->num_inputs = 1;
layer_132->num_outputs = 1;

auto layer_133 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[133]);
layer_133->activation = Activations::RELU;
layer_133->type = Layers::ACTIVATION;
layer_133->size = 0;
layer_133->data = nullptr;
layer_133->input_size = -1;
layer_133->output_size = -1;
layer_133->props = Mapper{.maxvalue = 6.0};
layer_133->num_inputs = 1;
layer_133->num_outputs = 1;

auto layer_134 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[134]);
layer_134->activation = Activations::NONE;
layer_134->type = Layers::DEPTHWISE_CONV2D;
layer_134->size = 3 * 3 * 576 * 1 * sizeof(float);
layer_134->data = nullptr;
layer_134->input_size = -1;
layer_134->output_size = -1;
layer_134->num_inputs = 1;
layer_134->num_outputs = 1;
layer_134->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 576, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_135 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[135]);
layer_135->activation = Activations::NONE;
layer_135->type = Layers::MULTIPLIER;
layer_135->size = 576 * sizeof(float);
layer_135->data = nullptr;
layer_135->input_size = -1;
layer_135->output_size = -1;
layer_135->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_135->num_inputs = 1;
layer_135->num_outputs = 1;
auto layer_136 = std::dynamic_pointer_cast<Layer<Add>>(layers_[136]);
layer_136->activation = Activations::NONE;
layer_136->type = Layers::ADD;
layer_136->size = 576 * sizeof(float);
layer_136->input_size = -1;
layer_136->output_size = -1;
layer_136->data = nullptr;
layer_136->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_136->num_inputs = 1;
layer_136->num_outputs = 1;

auto layer_137 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[137]);
layer_137->activation = Activations::RELU;
layer_137->type = Layers::ACTIVATION;
layer_137->size = 0;
layer_137->data = nullptr;
layer_137->input_size = -1;
layer_137->output_size = -1;
layer_137->props = Mapper{.maxvalue = 6.0};
layer_137->num_inputs = 1;
layer_137->num_outputs = 1;

auto layer_138 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[138]);
layer_138->activation = Activations::NONE;
layer_138->type = Layers::CONV2D;
layer_138->size = 1 * 1 * 576 * 96 * sizeof(float);
layer_138->data = nullptr;
layer_138->input_size = -1;
layer_138->output_size = -1;
layer_138->num_inputs = 1;
layer_138->num_outputs = 1;
layer_138->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 576, 96},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_139 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[139]);
layer_139->activation = Activations::NONE;
layer_139->type = Layers::MULTIPLIER;
layer_139->size = 96 * sizeof(float);
layer_139->data = nullptr;
layer_139->input_size = -1;
layer_139->output_size = -1;
layer_139->props = Multiplier{
    .num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_139->num_inputs = 1;
layer_139->num_outputs = 1;
auto layer_140 = std::dynamic_pointer_cast<Layer<Add>>(layers_[140]);
layer_140->activation = Activations::NONE;
layer_140->type = Layers::ADD;
layer_140->size = 96 * sizeof(float);
layer_140->input_size = -1;
layer_140->output_size = -1;
layer_140->data = nullptr;
layer_140->props =
    Add{.num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_140->num_inputs = 1;
layer_140->num_outputs = 1;

auto layer_141 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[141]);
layer_141->activation = Activations::NONE;
layer_141->type = Layers::LAYERWISE;
layer_141->size = 0;
layer_141->input_size = -1;
layer_141->output_size = -1;
layer_141->data = nullptr;
layer_141->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_141->num_inputs = 2;
layer_141->num_outputs = 1;

auto layer_142 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[142]);
layer_142->activation = Activations::NONE;
layer_142->type = Layers::CONV2D;
layer_142->size = 1 * 1 * 96 * 576 * sizeof(float);
layer_142->data = nullptr;
layer_142->input_size = -1;
layer_142->output_size = -1;
layer_142->num_inputs = 1;
layer_142->num_outputs = 1;
layer_142->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 96, 576},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_143 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[143]);
layer_143->activation = Activations::NONE;
layer_143->type = Layers::MULTIPLIER;
layer_143->size = 576 * sizeof(float);
layer_143->data = nullptr;
layer_143->input_size = -1;
layer_143->output_size = -1;
layer_143->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_143->num_inputs = 1;
layer_143->num_outputs = 1;
auto layer_144 = std::dynamic_pointer_cast<Layer<Add>>(layers_[144]);
layer_144->activation = Activations::NONE;
layer_144->type = Layers::ADD;
layer_144->size = 576 * sizeof(float);
layer_144->input_size = -1;
layer_144->output_size = -1;
layer_144->data = nullptr;
layer_144->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_144->num_inputs = 1;
layer_144->num_outputs = 1;

auto layer_145 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[145]);
layer_145->activation = Activations::RELU;
layer_145->type = Layers::ACTIVATION;
layer_145->size = 0;
layer_145->data = nullptr;
layer_145->input_size = -1;
layer_145->output_size = -1;
layer_145->props = Mapper{.maxvalue = 6.0};
layer_145->num_inputs = 1;
layer_145->num_outputs = 1;

auto layer_146 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[146]);
layer_146->activation = Activations::NONE;
layer_146->type = Layers::DEPTHWISE_CONV2D;
layer_146->size = 3 * 3 * 576 * 1 * sizeof(float);
layer_146->data = nullptr;
layer_146->input_size = -1;
layer_146->output_size = -1;
layer_146->num_inputs = 1;
layer_146->num_outputs = 1;
layer_146->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 576, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_147 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[147]);
layer_147->activation = Activations::NONE;
layer_147->type = Layers::MULTIPLIER;
layer_147->size = 576 * sizeof(float);
layer_147->data = nullptr;
layer_147->input_size = -1;
layer_147->output_size = -1;
layer_147->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_147->num_inputs = 1;
layer_147->num_outputs = 1;
auto layer_148 = std::dynamic_pointer_cast<Layer<Add>>(layers_[148]);
layer_148->activation = Activations::NONE;
layer_148->type = Layers::ADD;
layer_148->size = 576 * sizeof(float);
layer_148->input_size = -1;
layer_148->output_size = -1;
layer_148->data = nullptr;
layer_148->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_148->num_inputs = 1;
layer_148->num_outputs = 1;

auto layer_149 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[149]);
layer_149->activation = Activations::RELU;
layer_149->type = Layers::ACTIVATION;
layer_149->size = 0;
layer_149->data = nullptr;
layer_149->input_size = -1;
layer_149->output_size = -1;
layer_149->props = Mapper{.maxvalue = 6.0};
layer_149->num_inputs = 1;
layer_149->num_outputs = 1;

auto layer_150 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[150]);
layer_150->activation = Activations::NONE;
layer_150->type = Layers::CONV2D;
layer_150->size = 1 * 1 * 576 * 96 * sizeof(float);
layer_150->data = nullptr;
layer_150->input_size = -1;
layer_150->output_size = -1;
layer_150->num_inputs = 1;
layer_150->num_outputs = 1;
layer_150->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 576, 96},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_151 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[151]);
layer_151->activation = Activations::NONE;
layer_151->type = Layers::MULTIPLIER;
layer_151->size = 96 * sizeof(float);
layer_151->data = nullptr;
layer_151->input_size = -1;
layer_151->output_size = -1;
layer_151->props = Multiplier{
    .num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_151->num_inputs = 1;
layer_151->num_outputs = 1;
auto layer_152 = std::dynamic_pointer_cast<Layer<Add>>(layers_[152]);
layer_152->activation = Activations::NONE;
layer_152->type = Layers::ADD;
layer_152->size = 96 * sizeof(float);
layer_152->input_size = -1;
layer_152->output_size = -1;
layer_152->data = nullptr;
layer_152->props =
    Add{.num_dimensions = 1, .dimensions = {96}, .preceding = Layers::CONV2D};
layer_152->num_inputs = 1;
layer_152->num_outputs = 1;

auto layer_153 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[153]);
layer_153->activation = Activations::NONE;
layer_153->type = Layers::LAYERWISE;
layer_153->size = 0;
layer_153->input_size = -1;
layer_153->output_size = -1;
layer_153->data = nullptr;
layer_153->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_153->num_inputs = 2;
layer_153->num_outputs = 1;

auto layer_154 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[154]);
layer_154->activation = Activations::NONE;
layer_154->type = Layers::CONV2D;
layer_154->size = 1 * 1 * 96 * 576 * sizeof(float);
layer_154->data = nullptr;
layer_154->input_size = -1;
layer_154->output_size = -1;
layer_154->num_inputs = 1;
layer_154->num_outputs = 1;
layer_154->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 96, 576},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_155 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[155]);
layer_155->activation = Activations::NONE;
layer_155->type = Layers::MULTIPLIER;
layer_155->size = 576 * sizeof(float);
layer_155->data = nullptr;
layer_155->input_size = -1;
layer_155->output_size = -1;
layer_155->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_155->num_inputs = 1;
layer_155->num_outputs = 1;
auto layer_156 = std::dynamic_pointer_cast<Layer<Add>>(layers_[156]);
layer_156->activation = Activations::NONE;
layer_156->type = Layers::ADD;
layer_156->size = 576 * sizeof(float);
layer_156->input_size = -1;
layer_156->output_size = -1;
layer_156->data = nullptr;
layer_156->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_156->num_inputs = 1;
layer_156->num_outputs = 1;

auto layer_157 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[157]);
layer_157->activation = Activations::RELU;
layer_157->type = Layers::ACTIVATION;
layer_157->size = 0;
layer_157->data = nullptr;
layer_157->input_size = -1;
layer_157->output_size = -1;
layer_157->props = Mapper{.maxvalue = 6.0};
layer_157->num_inputs = 1;
layer_157->num_outputs = 1;

auto layer_158 = std::dynamic_pointer_cast<Layer<Padding>>(layers_[158]);
layer_158->activation = Activations::NONE;
layer_158->type = Layers::PADDING;
layer_158->size = 0 * sizeof(float);
layer_158->input_size = -1;
layer_158->output_size = -1;
layer_158->data = nullptr;
layer_158->props = Padding{.num_dimensions = 2, .padding = {0, 1, 0, 1}};
layer_158->num_inputs = 1;
layer_158->num_outputs = 1;

auto layer_159 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[159]);
layer_159->activation = Activations::NONE;
layer_159->type = Layers::DEPTHWISE_CONV2D;
layer_159->size = 3 * 3 * 576 * 1 * sizeof(float);
layer_159->data = nullptr;
layer_159->input_size = -1;
layer_159->output_size = -1;
layer_159->num_inputs = 1;
layer_159->num_outputs = 1;
layer_159->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 576, 1},
                               .strides = {2, 2, 1, 1},
                               .padding = {0, 0, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_160 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[160]);
layer_160->activation = Activations::NONE;
layer_160->type = Layers::MULTIPLIER;
layer_160->size = 576 * sizeof(float);
layer_160->data = nullptr;
layer_160->input_size = -1;
layer_160->output_size = -1;
layer_160->props = Multiplier{
    .num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_160->num_inputs = 1;
layer_160->num_outputs = 1;
auto layer_161 = std::dynamic_pointer_cast<Layer<Add>>(layers_[161]);
layer_161->activation = Activations::NONE;
layer_161->type = Layers::ADD;
layer_161->size = 576 * sizeof(float);
layer_161->input_size = -1;
layer_161->output_size = -1;
layer_161->data = nullptr;
layer_161->props =
    Add{.num_dimensions = 1, .dimensions = {576}, .preceding = Layers::CONV2D};
layer_161->num_inputs = 1;
layer_161->num_outputs = 1;

auto layer_162 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[162]);
layer_162->activation = Activations::RELU;
layer_162->type = Layers::ACTIVATION;
layer_162->size = 0;
layer_162->data = nullptr;
layer_162->input_size = -1;
layer_162->output_size = -1;
layer_162->props = Mapper{.maxvalue = 6.0};
layer_162->num_inputs = 1;
layer_162->num_outputs = 1;

auto layer_163 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[163]);
layer_163->activation = Activations::NONE;
layer_163->type = Layers::CONV2D;
layer_163->size = 1 * 1 * 576 * 160 * sizeof(float);
layer_163->data = nullptr;
layer_163->input_size = -1;
layer_163->output_size = -1;
layer_163->num_inputs = 1;
layer_163->num_outputs = 1;
layer_163->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 576, 160},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_164 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[164]);
layer_164->activation = Activations::NONE;
layer_164->type = Layers::MULTIPLIER;
layer_164->size = 160 * sizeof(float);
layer_164->data = nullptr;
layer_164->input_size = -1;
layer_164->output_size = -1;
layer_164->props = Multiplier{
    .num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_164->num_inputs = 1;
layer_164->num_outputs = 1;
auto layer_165 = std::dynamic_pointer_cast<Layer<Add>>(layers_[165]);
layer_165->activation = Activations::NONE;
layer_165->type = Layers::ADD;
layer_165->size = 160 * sizeof(float);
layer_165->input_size = -1;
layer_165->output_size = -1;
layer_165->data = nullptr;
layer_165->props =
    Add{.num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_165->num_inputs = 1;
layer_165->num_outputs = 1;

auto layer_166 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[166]);
layer_166->activation = Activations::NONE;
layer_166->type = Layers::CONV2D;
layer_166->size = 1 * 1 * 160 * 960 * sizeof(float);
layer_166->data = nullptr;
layer_166->input_size = -1;
layer_166->output_size = -1;
layer_166->num_inputs = 1;
layer_166->num_outputs = 1;
layer_166->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 160, 960},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_167 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[167]);
layer_167->activation = Activations::NONE;
layer_167->type = Layers::MULTIPLIER;
layer_167->size = 960 * sizeof(float);
layer_167->data = nullptr;
layer_167->input_size = -1;
layer_167->output_size = -1;
layer_167->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_167->num_inputs = 1;
layer_167->num_outputs = 1;
auto layer_168 = std::dynamic_pointer_cast<Layer<Add>>(layers_[168]);
layer_168->activation = Activations::NONE;
layer_168->type = Layers::ADD;
layer_168->size = 960 * sizeof(float);
layer_168->input_size = -1;
layer_168->output_size = -1;
layer_168->data = nullptr;
layer_168->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_168->num_inputs = 1;
layer_168->num_outputs = 1;

auto layer_169 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[169]);
layer_169->activation = Activations::RELU;
layer_169->type = Layers::ACTIVATION;
layer_169->size = 0;
layer_169->data = nullptr;
layer_169->input_size = -1;
layer_169->output_size = -1;
layer_169->props = Mapper{.maxvalue = 6.0};
layer_169->num_inputs = 1;
layer_169->num_outputs = 1;

auto layer_170 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[170]);
layer_170->activation = Activations::NONE;
layer_170->type = Layers::DEPTHWISE_CONV2D;
layer_170->size = 3 * 3 * 960 * 1 * sizeof(float);
layer_170->data = nullptr;
layer_170->input_size = -1;
layer_170->output_size = -1;
layer_170->num_inputs = 1;
layer_170->num_outputs = 1;
layer_170->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 960, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_171 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[171]);
layer_171->activation = Activations::NONE;
layer_171->type = Layers::MULTIPLIER;
layer_171->size = 960 * sizeof(float);
layer_171->data = nullptr;
layer_171->input_size = -1;
layer_171->output_size = -1;
layer_171->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_171->num_inputs = 1;
layer_171->num_outputs = 1;
auto layer_172 = std::dynamic_pointer_cast<Layer<Add>>(layers_[172]);
layer_172->activation = Activations::NONE;
layer_172->type = Layers::ADD;
layer_172->size = 960 * sizeof(float);
layer_172->input_size = -1;
layer_172->output_size = -1;
layer_172->data = nullptr;
layer_172->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_172->num_inputs = 1;
layer_172->num_outputs = 1;

auto layer_173 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[173]);
layer_173->activation = Activations::RELU;
layer_173->type = Layers::ACTIVATION;
layer_173->size = 0;
layer_173->data = nullptr;
layer_173->input_size = -1;
layer_173->output_size = -1;
layer_173->props = Mapper{.maxvalue = 6.0};
layer_173->num_inputs = 1;
layer_173->num_outputs = 1;

auto layer_174 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[174]);
layer_174->activation = Activations::NONE;
layer_174->type = Layers::CONV2D;
layer_174->size = 1 * 1 * 960 * 160 * sizeof(float);
layer_174->data = nullptr;
layer_174->input_size = -1;
layer_174->output_size = -1;
layer_174->num_inputs = 1;
layer_174->num_outputs = 1;
layer_174->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 960, 160},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_175 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[175]);
layer_175->activation = Activations::NONE;
layer_175->type = Layers::MULTIPLIER;
layer_175->size = 160 * sizeof(float);
layer_175->data = nullptr;
layer_175->input_size = -1;
layer_175->output_size = -1;
layer_175->props = Multiplier{
    .num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_175->num_inputs = 1;
layer_175->num_outputs = 1;
auto layer_176 = std::dynamic_pointer_cast<Layer<Add>>(layers_[176]);
layer_176->activation = Activations::NONE;
layer_176->type = Layers::ADD;
layer_176->size = 160 * sizeof(float);
layer_176->input_size = -1;
layer_176->output_size = -1;
layer_176->data = nullptr;
layer_176->props =
    Add{.num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_176->num_inputs = 1;
layer_176->num_outputs = 1;

auto layer_177 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[177]);
layer_177->activation = Activations::NONE;
layer_177->type = Layers::LAYERWISE;
layer_177->size = 0;
layer_177->input_size = -1;
layer_177->output_size = -1;
layer_177->data = nullptr;
layer_177->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_177->num_inputs = 2;
layer_177->num_outputs = 1;

auto layer_178 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[178]);
layer_178->activation = Activations::NONE;
layer_178->type = Layers::CONV2D;
layer_178->size = 1 * 1 * 160 * 960 * sizeof(float);
layer_178->data = nullptr;
layer_178->input_size = -1;
layer_178->output_size = -1;
layer_178->num_inputs = 1;
layer_178->num_outputs = 1;
layer_178->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 160, 960},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_179 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[179]);
layer_179->activation = Activations::NONE;
layer_179->type = Layers::MULTIPLIER;
layer_179->size = 960 * sizeof(float);
layer_179->data = nullptr;
layer_179->input_size = -1;
layer_179->output_size = -1;
layer_179->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_179->num_inputs = 1;
layer_179->num_outputs = 1;
auto layer_180 = std::dynamic_pointer_cast<Layer<Add>>(layers_[180]);
layer_180->activation = Activations::NONE;
layer_180->type = Layers::ADD;
layer_180->size = 960 * sizeof(float);
layer_180->input_size = -1;
layer_180->output_size = -1;
layer_180->data = nullptr;
layer_180->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_180->num_inputs = 1;
layer_180->num_outputs = 1;

auto layer_181 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[181]);
layer_181->activation = Activations::RELU;
layer_181->type = Layers::ACTIVATION;
layer_181->size = 0;
layer_181->data = nullptr;
layer_181->input_size = -1;
layer_181->output_size = -1;
layer_181->props = Mapper{.maxvalue = 6.0};
layer_181->num_inputs = 1;
layer_181->num_outputs = 1;

auto layer_182 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[182]);
layer_182->activation = Activations::NONE;
layer_182->type = Layers::DEPTHWISE_CONV2D;
layer_182->size = 3 * 3 * 960 * 1 * sizeof(float);
layer_182->data = nullptr;
layer_182->input_size = -1;
layer_182->output_size = -1;
layer_182->num_inputs = 1;
layer_182->num_outputs = 1;
layer_182->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 960, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_183 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[183]);
layer_183->activation = Activations::NONE;
layer_183->type = Layers::MULTIPLIER;
layer_183->size = 960 * sizeof(float);
layer_183->data = nullptr;
layer_183->input_size = -1;
layer_183->output_size = -1;
layer_183->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_183->num_inputs = 1;
layer_183->num_outputs = 1;
auto layer_184 = std::dynamic_pointer_cast<Layer<Add>>(layers_[184]);
layer_184->activation = Activations::NONE;
layer_184->type = Layers::ADD;
layer_184->size = 960 * sizeof(float);
layer_184->input_size = -1;
layer_184->output_size = -1;
layer_184->data = nullptr;
layer_184->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_184->num_inputs = 1;
layer_184->num_outputs = 1;

auto layer_185 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[185]);
layer_185->activation = Activations::RELU;
layer_185->type = Layers::ACTIVATION;
layer_185->size = 0;
layer_185->data = nullptr;
layer_185->input_size = -1;
layer_185->output_size = -1;
layer_185->props = Mapper{.maxvalue = 6.0};
layer_185->num_inputs = 1;
layer_185->num_outputs = 1;

auto layer_186 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[186]);
layer_186->activation = Activations::NONE;
layer_186->type = Layers::CONV2D;
layer_186->size = 1 * 1 * 960 * 160 * sizeof(float);
layer_186->data = nullptr;
layer_186->input_size = -1;
layer_186->output_size = -1;
layer_186->num_inputs = 1;
layer_186->num_outputs = 1;
layer_186->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 960, 160},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_187 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[187]);
layer_187->activation = Activations::NONE;
layer_187->type = Layers::MULTIPLIER;
layer_187->size = 160 * sizeof(float);
layer_187->data = nullptr;
layer_187->input_size = -1;
layer_187->output_size = -1;
layer_187->props = Multiplier{
    .num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_187->num_inputs = 1;
layer_187->num_outputs = 1;
auto layer_188 = std::dynamic_pointer_cast<Layer<Add>>(layers_[188]);
layer_188->activation = Activations::NONE;
layer_188->type = Layers::ADD;
layer_188->size = 160 * sizeof(float);
layer_188->input_size = -1;
layer_188->output_size = -1;
layer_188->data = nullptr;
layer_188->props =
    Add{.num_dimensions = 1, .dimensions = {160}, .preceding = Layers::CONV2D};
layer_188->num_inputs = 1;
layer_188->num_outputs = 1;

auto layer_189 = std::dynamic_pointer_cast<Layer<Layerwise>>(layers_[189]);
layer_189->activation = Activations::NONE;
layer_189->type = Layers::LAYERWISE;
layer_189->size = 0;
layer_189->input_size = -1;
layer_189->output_size = -1;
layer_189->data = nullptr;
layer_189->props = Layerwise{.initial_value = 0.f, .operation = Layers::ADD};
layer_189->num_inputs = 2;
layer_189->num_outputs = 1;

auto layer_190 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[190]);
layer_190->activation = Activations::NONE;
layer_190->type = Layers::CONV2D;
layer_190->size = 1 * 1 * 160 * 960 * sizeof(float);
layer_190->data = nullptr;
layer_190->input_size = -1;
layer_190->output_size = -1;
layer_190->num_inputs = 1;
layer_190->num_outputs = 1;
layer_190->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 160, 960},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_191 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[191]);
layer_191->activation = Activations::NONE;
layer_191->type = Layers::MULTIPLIER;
layer_191->size = 960 * sizeof(float);
layer_191->data = nullptr;
layer_191->input_size = -1;
layer_191->output_size = -1;
layer_191->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_191->num_inputs = 1;
layer_191->num_outputs = 1;
auto layer_192 = std::dynamic_pointer_cast<Layer<Add>>(layers_[192]);
layer_192->activation = Activations::NONE;
layer_192->type = Layers::ADD;
layer_192->size = 960 * sizeof(float);
layer_192->input_size = -1;
layer_192->output_size = -1;
layer_192->data = nullptr;
layer_192->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_192->num_inputs = 1;
layer_192->num_outputs = 1;

auto layer_193 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[193]);
layer_193->activation = Activations::RELU;
layer_193->type = Layers::ACTIVATION;
layer_193->size = 0;
layer_193->data = nullptr;
layer_193->input_size = -1;
layer_193->output_size = -1;
layer_193->props = Mapper{.maxvalue = 6.0};
layer_193->num_inputs = 1;
layer_193->num_outputs = 1;

auto layer_194 = std::dynamic_pointer_cast<Layer<DepthConv2D>>(layers_[194]);
layer_194->activation = Activations::NONE;
layer_194->type = Layers::DEPTHWISE_CONV2D;
layer_194->size = 3 * 3 * 960 * 1 * sizeof(float);
layer_194->data = nullptr;
layer_194->input_size = -1;
layer_194->output_size = -1;
layer_194->num_inputs = 1;
layer_194->num_outputs = 1;
layer_194->props = DepthConv2D{.num_dimensions = 3,
                               .dimensions = {3, 3, 960, 1},
                               .strides = {1, 1, 1, 1},
                               .padding = {1, 1, 0, 0},
                               .dilatations = {1, 1, 1, 1}};

auto layer_195 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[195]);
layer_195->activation = Activations::NONE;
layer_195->type = Layers::MULTIPLIER;
layer_195->size = 960 * sizeof(float);
layer_195->data = nullptr;
layer_195->input_size = -1;
layer_195->output_size = -1;
layer_195->props = Multiplier{
    .num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_195->num_inputs = 1;
layer_195->num_outputs = 1;
auto layer_196 = std::dynamic_pointer_cast<Layer<Add>>(layers_[196]);
layer_196->activation = Activations::NONE;
layer_196->type = Layers::ADD;
layer_196->size = 960 * sizeof(float);
layer_196->input_size = -1;
layer_196->output_size = -1;
layer_196->data = nullptr;
layer_196->props =
    Add{.num_dimensions = 1, .dimensions = {960}, .preceding = Layers::CONV2D};
layer_196->num_inputs = 1;
layer_196->num_outputs = 1;

auto layer_197 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[197]);
layer_197->activation = Activations::RELU;
layer_197->type = Layers::ACTIVATION;
layer_197->size = 0;
layer_197->data = nullptr;
layer_197->input_size = -1;
layer_197->output_size = -1;
layer_197->props = Mapper{.maxvalue = 6.0};
layer_197->num_inputs = 1;
layer_197->num_outputs = 1;

auto layer_198 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[198]);
layer_198->activation = Activations::NONE;
layer_198->type = Layers::CONV2D;
layer_198->size = 1 * 1 * 960 * 320 * sizeof(float);
layer_198->data = nullptr;
layer_198->input_size = -1;
layer_198->output_size = -1;
layer_198->num_inputs = 1;
layer_198->num_outputs = 1;
layer_198->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 960, 320},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_199 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[199]);
layer_199->activation = Activations::NONE;
layer_199->type = Layers::MULTIPLIER;
layer_199->size = 320 * sizeof(float);
layer_199->data = nullptr;
layer_199->input_size = -1;
layer_199->output_size = -1;
layer_199->props = Multiplier{
    .num_dimensions = 1, .dimensions = {320}, .preceding = Layers::CONV2D};
layer_199->num_inputs = 1;
layer_199->num_outputs = 1;
auto layer_200 = std::dynamic_pointer_cast<Layer<Add>>(layers_[200]);
layer_200->activation = Activations::NONE;
layer_200->type = Layers::ADD;
layer_200->size = 320 * sizeof(float);
layer_200->input_size = -1;
layer_200->output_size = -1;
layer_200->data = nullptr;
layer_200->props =
    Add{.num_dimensions = 1, .dimensions = {320}, .preceding = Layers::CONV2D};
layer_200->num_inputs = 1;
layer_200->num_outputs = 1;

auto layer_201 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[201]);
layer_201->activation = Activations::NONE;
layer_201->type = Layers::CONV2D;
layer_201->size = 1 * 1 * 320 * 1280 * sizeof(float);
layer_201->data = nullptr;
layer_201->input_size = -1;
layer_201->output_size = -1;
layer_201->num_inputs = 1;
layer_201->num_outputs = 1;
layer_201->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {1, 1, 320, 1280},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};

auto layer_202 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[202]);
layer_202->activation = Activations::NONE;
layer_202->type = Layers::MULTIPLIER;
layer_202->size = 1280 * sizeof(float);
layer_202->data = nullptr;
layer_202->input_size = -1;
layer_202->output_size = -1;
layer_202->props = Multiplier{
    .num_dimensions = 1, .dimensions = {1280}, .preceding = Layers::CONV2D};
layer_202->num_inputs = 1;
layer_202->num_outputs = 1;
auto layer_203 = std::dynamic_pointer_cast<Layer<Add>>(layers_[203]);
layer_203->activation = Activations::NONE;
layer_203->type = Layers::ADD;
layer_203->size = 1280 * sizeof(float);
layer_203->input_size = -1;
layer_203->output_size = -1;
layer_203->data = nullptr;
layer_203->props =
    Add{.num_dimensions = 1, .dimensions = {1280}, .preceding = Layers::CONV2D};
layer_203->num_inputs = 1;
layer_203->num_outputs = 1;

auto layer_204 = std::dynamic_pointer_cast<Layer<Mapper>>(layers_[204]);
layer_204->activation = Activations::RELU;
layer_204->type = Layers::ACTIVATION;
layer_204->size = 0;
layer_204->data = nullptr;
layer_204->input_size = -1;
layer_204->output_size = -1;
layer_204->props = Mapper{.maxvalue = 6.0};
layer_204->num_inputs = 1;
layer_204->num_outputs = 1;

auto layer_205 = std::dynamic_pointer_cast<Layer<Pooling>>(layers_[205]);
layer_205->activation = Activations::NONE;
layer_205->type = Layers::AVG_POOL;
layer_205->size = 0;
layer_205->input_size = -1;
layer_205->output_size = -1;
layer_205->data = nullptr;
layer_205->props =
    Pooling{.num_dimensions = 2, .poolings = {7, 7}, .strides = {1, 1}};
layer_205->num_inputs = 1;
layer_205->num_outputs = 1;

auto layer_206 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[206]);
layer_206->activation = Activations::NONE;
layer_206->type = Layers::DENSE;
layer_206->size = 1280 * 1000 * sizeof(float);
layer_206->input_size = 1280;
layer_206->output_size = 1000;
layer_206->data = nullptr;
layer_206->props = Dense{.num_dimensions = 2, .dimensions = {1280, 1000}};
layer_206->num_inputs = 1;
layer_206->num_outputs = 1;

auto layer_207 = std::dynamic_pointer_cast<Layer<Add>>(layers_[207]);
layer_207->activation = Activations::NONE;
layer_207->type = Layers::ADD;
layer_207->size = 1000 * sizeof(float);
layer_207->input_size = -1;
layer_207->output_size = 1000;
layer_207->data = nullptr;
layer_207->props =
    Add{.num_dimensions = 1, .dimensions = {1000}, .preceding = Layers::DENSE};
layer_207->num_inputs = 1;
layer_207->num_outputs = 1;

auto layer_208 =
    std::dynamic_pointer_cast<Layer<ReductionActivation>>(layers_[208]);
layer_208->activation = Activations::SOFTMAX;
layer_208->type = Layers::REDUCT_ACTIVATION;
layer_208->size = 0;
layer_208->input_size = -1;
layer_208->output_size = -1;
layer_208->data = nullptr;
layer_208->num_inputs = 1;
layer_208->num_outputs = 1;
