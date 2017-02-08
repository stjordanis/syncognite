#include <cp-neural.h>

#include "testneural.h"

int verbose;
vector<string> actualtests {};

// Manual build:
// g++ -g -ggdb -I ../cpneural -I /usr/local/include/eigen3 testneural.cpp -L
// ../Build/cpneural/ -lcpneural -lpthread -o test

bool matCompT(MatrixN &m0, MatrixN &m1, string msg, floatN eps, int verbose=1) {
	if (m0.cols() != m1.cols() || m0.rows() != m1.rows()) {
		cerr << "  " << msg << ": Incompatible shapes " << shape(m0) << "!=" << shape(m1)
		     << endl;
		return false;
	}
	MatrixN d = m0 - m1;
	floatN dif = d.cwiseProduct(d).sum();
	if (dif < eps) {
		if (verbose>1) {
			cerr << "    " << msg << ", err=" << dif << endl;
		}
		return true;
	} else {
		IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
		if (verbose>2) {
			cerr << msg << " m0:" << endl << m0.format(CleanFmt) << endl;
			cerr << msg << " m1:" << endl << m1.format(CleanFmt) << endl;
			cerr << msg << "  ∂:" << endl << (m0 - m1).format(CleanFmt) << endl;
		}
		if (verbose>0) {
			cerr << "    " << msg << ", err=" << dif << endl;
		}
		return false;
	}
}

vector<string> testcases{};

bool getTestCases() {
	for (auto it : _syncogniteLayerFactory.mapl) {
		testcases.push_back(it.first);
	}
	for (auto tc : additionaltestcases) {
		testcases.push_back(tc);
	}
}

bool checkForTest(string tc) {
	if (std::find(actualtests.begin(), actualtests.end(), tc) == actualtests.end()) {
		if (verbose>2) cerr << "Ignoring testcase: " << tc << endl;
		return false;
	} else {
		return true;
	}
}

bool registerTest() {
	bool allOk = true;
	cerr << "Registered Layers:" << endl;
	int nr = 1;
	for (auto it : _syncogniteLayerFactory.mapl) {
		cerr << nr << ".: " << it.first << " ";
		t_layer_props_entry te = _syncogniteLayerFactory.mapprops[it.first];
		CpParams cp;
		cp.setPar("inputShape", std::vector<int>(te));
		Layer *l = CREATE_LAYER(it.first, cp) if (l->layerType == LT_NORMAL) {
			cerr << "normal layer" << endl;
		}
		else if (l->layerType == LT_LOSS) {
			cerr << "loss-layer (final)" << endl;
		}
		else {
			cerr << "unspecified layer -- ERROR!" << endl;
			allOk = false;
		}
		delete l;
		++nr;
	}
	return allOk;
}

int tFunc(VectorN x, int c) {
	floatN y = 0.0;
	for (unsigned j = 0; j < x.cols(); j++) {
		y += x(j) * x(j);
	}
	y = int(y * (floatN)c / 2.0) % c;

	// cerr << x << ":" << y << "," << int(y) << " ";
	return int(y);
}

bool trainTest() {
	bool allOk = true;
	CpParams cp;
	// int N=500,NV=50,NT=50,I=5,H=10,C=4;
	int N = 100, NV = 40, NT = 40, I = 5, H = 20, C = 4;
	cp.setPar("inputShape", vector<int>{I});
	cp.setPar("hidden", vector<int>{H, C});
	cp.setPar("init", "normal");
	cp.setPar("initfactor", (floatN)0.1);
	TwoLayerNet tln(cp);

	MatrixN X(N, I); // XXX: 1-I never used by tfunc...
	X.setRandom();
	MatrixN y(N, 1);
	for (unsigned i = 0; i < y.rows(); i++) {
		VectorN s = X.row(i);
		y(i, 0) = tFunc(s, C);
	}

	MatrixN Xv(NV, I);
	Xv.setRandom();
	MatrixN yv(NV, 1);
	for (unsigned i = 0; i < yv.rows(); i++)
		yv(i, 0) = tFunc(Xv.row(i), C);

	MatrixN Xt(NT, I);
	Xt.setRandom();
	MatrixN yt(NT, 1);
	for (unsigned i = 0; i < yt.rows(); i++)
		yt(i, 0) = tFunc(Xt.row(i), C);

	CpParams cpo("{verbose=false;epochs=300.0;batch_size=20;learning_rate=5e-2;"
	    "lr_decay=1.0;epsilon=1e-8;regularization=1e-3;maxthreads=4}");

	floatN train_err, test_err, val_err;

	t_cppl states {}, statesv {}, statest {};
	states["y"] = &y;
	statesv["y"] = &yv;
	statest["y"] = &yt;
	tln.train(X, &states, Xv, &statesv, "Adam", cpo);
	// tln.train(X, y, Xv, yv, "Sdg", cpo);
	train_err = tln.test(X, &states);
	val_err = tln.test(Xv, &statesv);
	test_err = tln.test(Xt, &statest);

	cerr << "Train-test, train-err=" << train_err << endl;
	cerr << "       validation-err=" << val_err << endl;
	cerr << "       final test-err=" << val_err << endl;
	if (test_err > 0.4 || val_err > 0.4 || train_err > 0.4 || test_err < -10.0 ||
	    val_err < -10.0 || train_err < -10.0)
		allOk = false;
	return allOk;
}

vector<string> failedTests{};

void registerTestResult(string testcase, string subtest, bool result, string message) {
	Color::Modifier red(Color::FG_RED);
	Color::Modifier green(Color::FG_GREEN);
	Color::Modifier def(Color::FG_DEFAULT);
	if (result) {
		if (verbose>1) cerr << "  " << green << testcase << ", " << subtest << ": Ok " << message << def << endl;
	} else {
		if (verbose>0) cerr << "  " << red << testcase << ", " << subtest << ": Error " << message << def << endl;
		failedTests.push_back(testcase + ": " + subtest);
	}
}

int doTests() {
    MatrixN yz=MatrixN(0,0);
    t_cppl s1;
    s1["y"] = &yz;
    floatN h, eps;

    bool allOk=true;
    Color::Modifier lblue(Color::FG_LIGHT_BLUE);
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    cerr << endl << endl << lblue << "Syncognite layer tests" << def << endl;
    if (verbose > 0) {
        cerr << "Active tests: ";
        for (auto at : actualtests) cerr << at << " ";
        cerr << endl;
    }
    MatrixN xr(10,20); // XXX: These two lines generate a side-effect
    xr.setRandom();    // XXX: that prevents numerical diff on Relu fail!

	if (checkForTest("Affine")) if (!testAffine(verbose)) allOk=false;
    if (checkForTest("Relu")) if (!testRelu(verbose)) allOk=false;
    if (checkForTest("Nonlinearity")) if (!testNonlinearity(verbose)) allOk=false;
    if (checkForTest("AffineRelu")) if (!testAffineRelu(verbose)) allOk=false;
    if (checkForTest("BatchNorm")) if (!testBatchNorm(verbose)) allOk=false;
    if (checkForTest("Dropout")) if (!testDropout(verbose)) allOk=false;
    if (checkForTest("Convolution")) if (!testConvolution(verbose)) allOk=false;
    if (checkForTest("Pooling")) if (!testPooling(verbose)) allOk=false;
    if (checkForTest("SpatialBatchNorm")) if (!testSpatialBatchNorm(verbose)) allOk=false;
    if (checkForTest("Svm")) if (!testSvm(verbose)) allOk=false;
    if (checkForTest("Softmax")) if (!testSoftmax(verbose)) allOk=false;
    if (checkForTest("TwoLayerNet")) if (!testTwoLayerNet(verbose)) allOk=false;




	// RNN
	int rnnN = 4; // N=4, D=5, H=6, T=7
	MatrixN xrnn(rnnN, 5 * 7);
	t_cppl rnstates;
	MatrixN h0(rnnN, 6);
	xrnn.setRandom();
	h0.setRandom();
	rnstates["rnn-h"] = &h0;
	//                    D,T
	RNN rnn("{name='rnn';inputShape=[5,7];H=6;N=4;noVectorizationTests=true;"
	    "nohupdate=true}");
	if (!rnn.selfTest(xrnn, &rnstates, 1e-4, 1e-4)) {
		allOk = false;
	}

	// LSTM
	int lstmN = 4; // N=4, D=5, H=6, T=7
	MatrixN xlstm(lstmN, 5 * 7);
	t_cppl lsstates;
	MatrixN hl0(lstmN, 6);
	xlstm.setRandom();
	hl0.setRandom();
	lsstates["lstm-h"] = &hl0;
	//                                 D,T
	LSTM lstm("{name='lstm';inputShape=[5,7];H=6;N=4;noVectorizationTests=true;"
	    "nohupdate=true}");
	if (!lstm.selfTest(xlstm, &lsstates, 1e-4, 1e-4)) {
		allOk = false;
	}

	// WordEmbedding
	int weN = 4, weT = 3, weV = 10, weD = 8;
	MatrixN xwe(weN, weT);
	MatrixN weW(weV, weD);
	xwe.setRandom();
	weW.setRandom();
	WordEmbedding we("{inputShape=[3];V=10;D=8;noVectorizationTests=true}");
	*(we.params["W"]) = weW;
	if (!we.selfTest(xwe, &s1, 1e-2, 1e-3)) {
		allOk = false;
	}

	// N=10; T=5; D=6; M=7
	// TemporalAffine
	// pct(CpParams("{inputShape=[30];T=5;D=6;M=7;noVectorizationTests=true}"));
	// // 30=T*D
	TemporalAffine pct(CpParams("{inputShape=[6,5];M=7}")); // T=5;D=6;30=T*D
	MatrixN xtt(10, 30);
	xtt.setRandom();
	if (!pct.selfTest(xtt, &s1)) {
		allOk = false;
	}

	// Temporal Softmax
	int tsmN = 10, tsmC = 4, Ttm = 4;
	CpParams tc1;
	tc1.setPar("inputShape", vector<int>{tsmC, Ttm});
	tc1.setPar("noVectorizationTests", (bool)true);
	TemporalSoftmax tmx(tc1);
	MatrixN txmx(tsmN, tsmC * Ttm);
	txmx.setRandom();
	MatrixN ty(tsmN, Ttm);
	for (unsigned i = 0; i < ty.size(); i++)
		ty(i) = (rand() % tsmC);
	h = 1e-2;
	if (h < CP_DEFAULT_NUM_H)
		h = CP_DEFAULT_NUM_H;
	eps = 1e-4;
	if (eps < CP_DEFAULT_NUM_EPS)
		eps = CP_DEFAULT_NUM_EPS;
	t_cppl states;
	states["y"] = &ty;
	if (!tmx.selfTest(txmx, &states, h, eps)) {
		allOk = false;
	}

	// LayerBlock1
	LayerBlock lb("{name='testblock'}");
	cerr << "LayerName for lb: " << lb.layerName << endl;
	lb.addLayer("Affine", "af1", "{inputShape=[10]}", {"input"});
	lb.addLayer("Relu", "rl1", "", {"af1"});
	lb.addLayer("Affine", "af2", "{hidden=10}", {"rl1"});
	lb.addLayer("Softmax", "sm1", "", {"af2"});
	if (!lb.checkTopology(true)) {
		allOk = false;
		cerr << red << "Topology-check for LayerBlock: ERROR." << def << endl;
	} else {
		cerr << green << "Topology-check for LayerBlock: ok." << def << endl;
	}
	MatrixN xml(5, 10);
	xml.setRandom();
	MatrixN yml(5, 1);
	for (unsigned i = 0; i < yml.rows(); i++)
		yml(i, 0) = (rand() % 10);

	h = 1e-3;
	if (h < CP_DEFAULT_NUM_H)
		h = CP_DEFAULT_NUM_H;
	eps = 1e-5;
	if (eps < CP_DEFAULT_NUM_EPS)
		eps = CP_DEFAULT_NUM_EPS;
	t_cppl lbstates;
	lbstates["y"] = &yml;
	// This is just a pain: slow:
	if (!lb.selfTest(xml, &lbstates, h, eps)) {
		allOk = false;
		cerr << red << "Numerical gradient for LayerBlock: ERROR." << def << endl;
	}

	cerr << "=== 2.: Test-data tests" << endl;

	if (checkRNNStepForward()) {
		cerr << green << "RNNForwardStep with test data: OK." << def << endl;
	} else {
		cerr << red << "RNNForwardStep with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkRNNStepBackward()) {
		cerr << green << "RNNBackwardStep with test data: OK." << def << endl;
	} else {
		cerr << red << "RNNBackwardStep with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkRNNForward()) {
		cerr << green << "RNNForward with test data: OK." << def << endl;
	} else {
		cerr << red << "RNNForward with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkRNNBackward()) {
		cerr << green << "RNNBackward with test data: OK." << def << endl;
	} else {
		cerr << red << "RNNBackward with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkLSTMStepForward()) {
		cerr << green << " LSTMForwardStep with test data: OK." << def << endl;
	} else {
		cerr << red << " LSTMForwardStep with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkLSTMStepBackward()) {
		cerr << green << " LSTMBackwardStep with test data: OK." << def << endl;
	} else {
		cerr << red << " LSTMBackwardStep with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkLSTMForward()) {
		cerr << green << " LSTMForward with test data: OK." << def << endl;
	} else {
		cerr << red << " LSTMForward with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkLSTMBackward()) {
		cerr << green << " LSTMBackward with test data: OK." << def << endl;
	} else {
		cerr << red << " LSTMBackward with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkWordEmbeddingForward()) {
		cerr << green << "WordEmbeddingForward with test data: OK." << def << endl;
	} else {
		cerr << red << "WordEmbeddingForward with test data: ERROR." << def << endl;
		allOk = false;
	}
	if (checkWordEmbeddingBackward()) {
		cerr << green << "WordEmbeddingBackward with test data: OK." << def << endl;
	} else {
		cerr << red << "WordEmbeddingBackward with test data: ERROR." << def
		     << endl;
		allOk = false;
	}

	if (checkTemporalAffineForward()) {
		cerr << green << "TemporalAffineForward with test data: OK." << def << endl;
	} else {
		cerr << red << "TemporalAffineForward with test data: ERROR." << def
		     << endl;
		allOk = false;
	}

	if (checkTemporalAffineBackward()) {
		cerr << green << "TemporalAffineBackward with test data: OK." << def
		     << endl;
	} else {
		cerr << red << "TemporalAffineBackward with test data: ERROR." << def
		     << endl;
		allOk = false;
	}

	if (checkTemporalSoftmaxLoss(0.1)) {
		cerr << green << "TemporalSoftmaxLoss with test data: OK." << def << endl;
	} else {
		cerr << red << "TemporalSoftmaxLoss with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (checkTemporalSoftmax()) {
		cerr << green << "TemporalSoftmax with test data: OK." << def << endl;
	} else {
		cerr << red << "TemporalSoftmax with test data: ERROR." << def << endl;
		allOk = false;
	}

	if (trainTest()) {
		cerr << green << "TrainTest: OK." << def << endl;
	} else {
		cerr << red << "TrainTest: ERROR." << def << endl;
		allOk = false;
	}

	/*
	    if (registerTest()) {
	        cerr << green << "RegisterTest: OK." << def << endl;
	    } else {
	        cerr << red << "RegisterTest: ERROR." << def << endl;
	        allOk=false;
	    }
	 */
	if (allOk) {
		cerr << green << "All tests ok." << def << endl;
	} else {
		cerr << red << "Tests failed." << def << endl;
	}

	return 0;
}


bool getArgs(int argc, char *argv[]) {
	if (argc == 1) {
		actualtests=testcases;
		return true;
	}
	for (int i = 1; i < argc; i++) {
		string a(argv[i]);
		if (a[0] == '-') {
			auto opt = a.substr(1);
			if (opt == "v")
				verbose = 1;
			else if (opt == "vv")
				verbose = 2;
			else if (opt == "vvv")
				verbose = 3;
			else {
				cerr << "Invalid option: " << opt << endl;
				cerr << "Valid options are: -v -vv -vvv (increasing verbosity)" << endl;
				return false;
			}
		} else {
            if (std::find(testcases.begin(), testcases.end(), a) == testcases.end()) {
				cerr << "No testcase defined for: " << a << endl;
				cerr << "Valid testcases are: ";
				for (auto tc : testcases)
					cerr << tc << " ";
				cerr << endl;
				return false;
			} else {
				actualtests.push_back(a);
			}
		}
	}
	if (actualtests.size() == 0)
		actualtests = testcases;
	return true;
}

int main(int argc, char *argv[]) {
	verbose=1;
	string name = "test";
	cpInitCompute(name, nullptr, 0);
	registerLayers();
	getTestCases();

	if (!getArgs(argc, argv)) {
		cpExitCompute();
		exit(-1);
	}

	if (verbose > 3) {
		cerr << "Testcases: ";
		for (auto tc : actualtests)
			cerr << tc << " ";
		cerr << endl;
	}

	int ret = 0;
	ret = doTests();
	cpExitCompute();
	return ret;
}
