#include "header.hpp"
#define BONGO_KEYPRESS_THRESHOLD 0.031

namespace osu {
    Json::Value left_key_value, right_key_value, key3, key4, key5, key6, smoke_key_value, wave_key_value, sunglass_key_value;
Json::Value dance_key_value, mirrorball_key_value, bgdown_key_value, lemon_face_key_value;
int osu_x, osu_y, osu_h, osu_v;
int offset_x, offset_y;
int horizontal, vertical;
int paw_r, paw_g, paw_b;
int paw_edge_r, paw_edge_g, paw_edge_b;
double dance_speed;
double mirrorball_speed;
double scale;
bool is_mouse, is_letterbox, is_left_handed;
sf::Sprite bg, bg_down, up, left, right, device, smoke, wave, sunglass, sunglass_r, bg_lemon_face;
sf::Sprite dance1, dance1_1, dance2, dance2_1, dance3, dance3_1, dance4, dance4_1, mirrorball1, mirrorball2, mirrorball3;

int key_state = 0;
bool left_key_state = false;
bool right_key_state = false;
bool key3_state = false;
bool key4_state = false;
bool key5_state = false;
bool key6_state = false;
bool wave_key_state = false;
double timer_left_key = -1;
double timer_right_key = -1;
double timer_wave_key = -1;

std::tuple<double, double> bezier(double ratio, std::vector<double> &points, int length) {
    double fact[22] = {0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0};
    int nn = (length / 2) - 1;
    double xx = 0;
    double yy = 0;

    for (int point = 0; point <= nn; point++) {
        double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
        xx += points[2 * point] * tmp;
        yy += points[2 * point + 1] * tmp;
    }

    return std::make_tuple(xx / 1000, yy / 1000);
}

bool init() {
    // getting configs
    is_mouse = data::cfg["osu"]["mouse"].asBool();

    paw_r = data::cfg["osu"]["paw"][0].asInt();
    paw_g = data::cfg["osu"]["paw"][1].asInt();
    paw_b = data::cfg["osu"]["paw"][2].asInt();

    paw_edge_r = data::cfg["osu"]["pawEdge"][0].asInt();
    paw_edge_g = data::cfg["osu"]["pawEdge"][1].asInt();
    paw_edge_b = data::cfg["osu"]["pawEdge"][2].asInt();
    
    dance_speed = data::cfg["osu"]["danceSpeed"].asDouble();
    mirrorball_speed = data::cfg["osu"]["mirrorballSpeed"].asDouble();


    bool chk[256];
    std::fill(chk, chk + 256, false);
    left_key_value = data::cfg["osu"]["key1"];
    for (Json::Value &v : left_key_value) {
        chk[v.asInt()] = true;
    }
    right_key_value = data::cfg["osu"]["key2"];
    for (Json::Value &v : right_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
	key3 = data::cfg["osu"]["key3"];
	for (Json::Value &v : key3) {
		if (chk[v.asInt()]) {
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}
	}
	key4 = data::cfg["osu"]["key4"];
	for (Json::Value &v : key4) {
		if (chk[v.asInt()]) {
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}
	}
	key5 = data::cfg["osu"]["key5"];
	for (Json::Value &v : key5) {
		if (chk[v.asInt()]) {
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}
	}
	key6 = data::cfg["osu"]["key6"];
	for (Json::Value &v : key5) {
		if (chk[v.asInt()]) {
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}
	}
    wave_key_value = data::cfg["osu"]["wave"];
    for (Json::Value &v : wave_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    smoke_key_value = data::cfg["osu"]["smoke"];
    for (Json::Value& v : smoke_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    sunglass_key_value = data::cfg["osu"]["sunglass"];
    for (Json::Value& v : smoke_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    dance_key_value = data::cfg["osu"]["dance"];
    for (Json::Value& v : dance_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    mirrorball_key_value = data::cfg["osu"]["mirrorball"];
    for (Json::Value& v : mirrorball_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }
    bgdown_key_value = data::cfg["osu"]["bg_down"];
    for (Json::Value& v : bgdown_key_value) {
        if (chk[v.asInt()]) {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    }

    is_letterbox = data::cfg["resolution"]["letterboxing"].asBool();
    osu_x = data::cfg["resolution"]["width"].asInt();
    osu_y = data::cfg["resolution"]["height"].asInt();
    osu_h = data::cfg["resolution"]["horizontalPosition"].asInt();
    osu_v = data::cfg["resolution"]["verticalPosition"].asInt();
    is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

    if (is_mouse) {
        offset_x = (data::cfg["decoration"]["offsetX"])[0].asInt();
        offset_y = (data::cfg["decoration"]["offsetY"])[0].asInt();
        scale = (data::cfg["decoration"]["scalar"])[0].asDouble();
    } else {
        offset_x = (data::cfg["decoration"]["offsetX"])[1].asInt();
        offset_y = (data::cfg["decoration"]["offsetY"])[1].asInt();
        scale = (data::cfg["decoration"]["scalar"])[1].asDouble();
    }

    // importing sprites
    up.setTexture(data::load_texture("img/osu/up.png"));
    left.setTexture(data::load_texture("img/osu/left.png"));
    right.setTexture(data::load_texture("img/osu/right.png"));
    wave.setTexture(data::load_texture("img/osu/wave.png"));
    if (is_mouse) {
        bg.setTexture(data::load_texture("img/osu/mousebg.png"));
        bg_down.setTexture(data::load_texture("img/osu/mousebg_down.png"));
        bg_lemon_face.setTexture(data::load_texture("img/osu/mousebg_lemon_face.png"));
        device.setTexture(data::load_texture("img/osu/mouse.png"), true);
    } else {
        bg.setTexture(data::load_texture("img/osu/tabletbg.png"));
        bg_down.setTexture(data::load_texture("img/osu/tabletbg.png"));
        bg_lemon_face.setTexture(data::load_texture("img/osu/tabletbg.png"));
        device.setTexture(data::load_texture("img/osu/tablet.png"), true);
    }
    smoke.setTexture(data::load_texture("img/osu/smoke.png"));
    
    sunglass.setTexture(data::load_texture("img/osu/sunglass.png"));
    sunglass_r.setTexture(data::load_texture("img/osu/sunglass_r.png"));
    dance1.setTexture(data::load_texture("img/osu/dance1.png"));
    dance1_1.setTexture(data::load_texture("img/osu/dance1_1.png"));
    dance2.setTexture(data::load_texture("img/osu/dance2.png"));
    dance2_1.setTexture(data::load_texture("img/osu/dance2_1.png"));
    dance3.setTexture(data::load_texture("img/osu/dance3.png"));
    dance3_1.setTexture(data::load_texture("img/osu/dance3_1.png"));
    dance4.setTexture(data::load_texture("img/osu/dance4.png"));
    dance4_1.setTexture(data::load_texture("img/osu/dance4_1.png"));

    mirrorball1.setTexture(data::load_texture("img/osu/mirrorball1.png"));
    mirrorball2.setTexture(data::load_texture("img/osu/mirrorball2.png"));
    mirrorball3.setTexture(data::load_texture("img/osu/mirrorball3.png"));

    device.setScale(scale, scale);

    // getting resolution
    RECT desktop;
    const HWND h_desktop = GetDesktopWindow();
    GetWindowRect(h_desktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;

    return true;
}
void sunglass_mirrorball(bool is_reverse) {
    //bool mirrorball_key = false;
    for (Json::Value& v : mirrorball_key_value) {
        if (GetKeyState(v.asInt()) & 0x0001) {
            //mirrorball_key = true;
            long time = clock() % (int)(1000 / mirrorball_speed);
            if (time > 666 / mirrorball_speed)
                window.draw(mirrorball1);
            else if (time > 333 / mirrorball_speed)
                window.draw(mirrorball2);
            else
                window.draw(mirrorball3);
        }
    }

    for (Json::Value& v : sunglass_key_value) {
        if (GetKeyState(v.asInt()) & 0x0001) {
            //mirrorball_key = true;
            if (!is_reverse)
                window.draw(sunglass);
            else
                window.draw(sunglass_r);
        }
    }
}

void draw() {
    // getting device resolution
    double letter_x, letter_y, s_height, s_width;


    bool dance_key = false;
    bool is_reverse = false;
    for (Json::Value& v : dance_key_value) {
        if (GetKeyState(v.asInt()) & 0x0001) {
            dance_key = true;
            long time = clock() % (int)(1000/dance_speed);
            if (time > 800/dance_speed)
                window.draw(dance1);
            else if (time > 750/ dance_speed)
                window.draw(dance1_1);
            else if (time >550/ dance_speed)
                window.draw(dance2);
            else if (time > 500 / dance_speed)
                window.draw(dance2_1);
            else {
                is_reverse = true;
                if (time > 300 / dance_speed)
                    window.draw(dance3);
                else if (time > 250 / dance_speed)
                    window.draw(dance3_1);
                else if (time > 50 / dance_speed)
                    window.draw(dance4);
                else
                    window.draw(dance4_1);
            }
        }
    }

    if (dance_key) {
        //bool mirrorball_key = false;
        sunglass_mirrorball(is_reverse);
        return;
    }
    

    HWND handle = GetForegroundWindow();
    if (handle) {
        TCHAR w_title[256];
        GetWindowText(handle, w_title, GetWindowTextLength(handle));
        std::string title = w_title;
        if (title.find("osu!") == 0) {
            RECT oblong;
            GetWindowRect(handle, &oblong);
            s_height = osu_y * 0.8;
            s_width = s_height * 4 / 3;
            if (!is_letterbox) {
                letter_x = oblong.left + ((oblong.right - oblong.left) - s_width) / 2;
                letter_y = oblong.top + osu_y * 0.117;
            } else {
                double l = (horizontal - osu_x) * (osu_h + 100) / 200.0;
                double r = l + osu_x;
                letter_x = l + ((r - l) - s_width) / 2;
                letter_y = (vertical - osu_y) * (osu_v + 100) / 200.0 + osu_y * 0.117;
            }
        } else {
            s_width = horizontal;
            s_height = vertical;
            letter_x = 0;
            letter_y = 0;
        }
    } else {
        s_width = horizontal;
        s_height = vertical;
        letter_x = 0;
        letter_y = 0;
    }
    double x, y;
    POINT point;
    if (GetCursorPos(&point)) {
        if (!is_letterbox) {
            letter_x = floor(1.0 * point.x / osu_x) * osu_x;
            letter_y = floor(1.0 * point.y / osu_y) * osu_y;
        }
        double fx = (1.0 * point.x - letter_x) / s_width;
        if (is_left_handed) {
            fx = 1 - fx;
        }
        double fy = (1.0 * point.y - letter_y) / s_height;
        fx = std::min(fx, 1.0);
        fx = std::max(fx, 0.0);
        fy = std::min(fy, 1.0);
        fy = std::max(fy, 0.0);
        x = -97 * fx + 44 * fy + 184;
        y = -76 * fx - 40 * fy + 324;
    }
    window.draw(bg);
    // drawing lemon face
    //사실 for문은 빠져도 됨
    //for (Json::Value& v : lemon_face_key_value) {
    if (GetKeyState(VK_PAUSE) & 0x0001) {
        window.draw(bg_lemon_face);
        //break;
    }
    //}
    
    // drawing down bg
    bool is_bg_down = false;
    for (Json::Value& v : bgdown_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            is_bg_down = true;
            window.draw(bg_down);
            break;
        }
    }

    // initializing pss and pss2 (kuvster's magic)
    int oof = 6;
    std::vector<double> pss = {211.0, 159.0};
    double dist = hypot(211 - x, 159 - y);
    double centreleft0 = 211 - 0.7237 * dist / 2;
    double centreleft1 = 159 + 0.69 * dist / 2;
    for (int i = 1; i < oof; i++) {
        std::vector<double> bez = {211, 159, centreleft0, centreleft1, x, y};
        auto [p0, p1] = bezier(1.0 * i / oof, bez, 6);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(x);
    pss.push_back(y);
    double a = y - centreleft1;
    double b = centreleft0 - x;
    double le = hypot(a, b);
    a = x + a / le * 60;
    b = y + b / le * 60;
    int a1 = 258;
    int a2 = 228;
    dist = hypot(a1 - a, a2 - b);
    double centreright0 = a1 - 0.6 * dist / 2;
    double centreright1 = a2 + 0.8 * dist / 2;
    int push = 20;
    double s = x - centreleft0;
    double t = y - centreleft1;
    le = hypot(s, t);
    s *= push / le;
    t *= push / le;
    double s2 = a - centreright0;
    double t2 = b - centreright1;
    le = hypot(s2, t2);
    s2 *= push / le;
    t2 *= push / le;
    for (int i = 1; i < oof; i++) {
        std::vector<double> bez = {x, y, x + s, y + t, a + s2, b + t2, a, b};
        auto [p0, p1] = bezier(1.0 * i / oof, bez, 8);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(a);
    pss.push_back(b);
    for (int i = oof - 1; i > 0; i--) {
        std::vector<double> bez = {1.0 * a1, 1.0 * a2, centreright0, centreright1, a, b};
        auto [p0, p1] = bezier(1.0 * i / oof, bez, 6);
        pss.push_back(p0);
        pss.push_back(p1);
    }
    pss.push_back(a1);
    pss.push_back(a2);
    double mpos0 = (a + x) / 2 - 52 - 15;
    double mpos1 = (b + y) / 2 - 34 + 5;
    //이게 x축 y축과 밀접한 관련이 있음
    double dx = -38;
    double dy = 0;
    if (is_bg_down) {
        dy = 7;
        up.setPosition(0.f, 7.f);
        left.setPosition(0.f, 7.f);
        right.setPosition(0.f, 7.f);
        smoke.setPosition(0.f, 7.f);
        wave.setPosition(0.f, 7.f);
        sunglass.setPosition(0.f, 7.f);
        sunglass_r.setPosition(0.f, 7.f);
    }
    else {
        up.setPosition(0.f, 0.f);
        left.setPosition(0.f, 0.f);
        right.setPosition(0.f, 0.f);
        smoke.setPosition(0.f, 0.f);
        wave.setPosition(0.f, 0.f);
        sunglass.setPosition(0.f, 0.f);
        sunglass_r.setPosition(0.f, 0.f);
    }


    const int iter = 25;

    std::vector<double> pss2 = {pss[0] + dx, pss[1] + dy};
    for (int i = 1; i < iter; i++) {
        auto [p0, p1] = bezier(1.0 * i / iter, pss, 38);
        pss2.push_back(p0 + dx);
        pss2.push_back(p1 + dy);
    }
    pss2.push_back(pss[36] + dx);
    pss2.push_back(pss[37] + dy);

    device.setPosition(mpos0 + dx + offset_x, mpos1 + dy + offset_y);

    // drawing mouse
    if (is_mouse) {
        window.draw(device);
    }

    // drawing arms
    sf::VertexArray fill(sf::TriangleStrip, 26);
    for (int i = 0; i < 26; i += 2) {
        fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
        fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
        fill[i].color = sf::Color(paw_r, paw_g, paw_b, 255);
        fill[i + 1].color = sf::Color(paw_r, paw_g, paw_b, 255);
    }
    window.draw(fill);

    // drawing first arm arc
    int shad = 77;
    sf::VertexArray edge(sf::TriangleStrip, 52);
    double width = 7;
    sf::CircleShape circ(width / 2);
    circ.setFillColor(sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad));
    circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ);
    for (int i = 0; i < 50; i += 2) {
        double vec0 = pss2[i] - pss2[i + 2];
        double vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge[i].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
        edge[i + 1].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
        width -= 0.08;
    }
    double vec0 = pss2[50] - pss2[48];
    double vec1 = pss2[51] - pss2[49];
    dist = hypot(vec0, vec1);
    edge[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge[50].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
    edge[51].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, shad);
    window.draw(edge);
    circ.setRadius(width / 2);
    circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ);

    // drawing second arm arc
    sf::VertexArray edge2(sf::TriangleStrip, 52);
    width = 6;
    sf::CircleShape circ2(width / 2);
    circ2.setFillColor(sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, 255));
    circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ2);
    for (int i = 0; i < 50; i += 2) {
        vec0 = pss2[i] - pss2[i + 2];
        vec1 = pss2[i + 1] - pss2[i + 3];
        double dist = hypot(vec0, vec1);
        edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
        edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
        edge2[i].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, 255);
        edge2[i + 1].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, 255);
        width -= 0.08;
    }
    vec0 = pss2[50] - pss2[48];
    vec1 = pss2[51] - pss2[49];
    dist = hypot(vec0, vec1);
    edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
    edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
    edge2[50].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, 255);
    edge2[51].color = sf::Color(paw_edge_r, paw_edge_g, paw_edge_b, 255);
    window.draw(edge2);
    circ2.setRadius(width / 2);
    circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ2);

    // drawing keypresses
    bool left_key = false;
    for (Json::Value &v : left_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            left_key = true;
            break;
        }
    }
    if (left_key) {
        if (!left_key_state) {
            key_state = 1;
            left_key_state = true;
        }
    } else {
        left_key_state = false;
    }

    bool right_key = false;
    for (Json::Value &v : right_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            right_key = true;
            break;
        }
    }
    if (right_key) {
        if (!right_key_state) {
            key_state = 2;
            right_key_state = true;
        }
    } else {
        right_key_state = false;
    }

	bool is_key3 = false;
	for (Json::Value &v : key3) {
		if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
			is_key3 = true;
			break;
		}
	}
	if (is_key3) {
		if (!key3_state) {
			key_state = 3;
			key3_state = true;
		}
	}
	else {
		key3_state = false;
	}

	bool is_key4 = false;
	for (Json::Value &v : key4) {
		if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
			is_key4 = true;
			break;
		}
	}
	if (is_key4) {
		if (!key4_state) {
			key_state = 4;
			key4_state = true;
		}
	}
	else {
		key4_state = false;
	}

	bool is_key5 = false;
	for (Json::Value &v : key5) {
		if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
			is_key5 = true;
			break;
		}
	}
	if (is_key5) {
		if (!key5_state) {
			key_state = 5;
			key5_state = true;
		}
	}
	else {
		key5_state = false;
	}

	bool is_key6 = false;
	for (Json::Value &v : key6) {
		if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
			is_key6 = true;
			break;
		}
	}
	if (is_key6) {
		if (!key6_state) {
			key_state = 6;
			key6_state = true;
		}
	}
	else {
		key6_state = false;
	}

    
    bool wave_key = false;
    for (Json::Value &v : wave_key_value) {
        if (GetKeyState(v.asInt()) & WM_KEYDOWN) {
            wave_key = true;
            break;
        }
    }
    if (wave_key) {
        if (!wave_key_state) {
            key_state = 7;
            wave_key_state = true;
        }
    } else {
        wave_key_state = false;
    }

    if (!left_key_state && !right_key_state && !key3_state && !key4_state && !key5_state && !key6_state && !wave_key_state) {
        key_state = 0;
        window.draw(up);
    }
    if (key_state == 1) {
        if ((clock() - std::max(timer_right_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            if (!is_left_handed) {
                window.draw(left);
            } else {
                window.draw(right);
            }
            timer_left_key = clock();
        } else {
            window.draw(up);
        }
    } else if (key_state == 2) {
        if ((clock() - std::max(timer_left_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            if (!is_left_handed) {
                window.draw(right);
            } else {
                window.draw(left);
            }
            timer_right_key = clock();
        } else {
            window.draw(up);
        }
		} else if (key_state == 3) {
        if ((clock() - std::max(timer_right_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
            if (!is_left_handed) {
                window.draw(left);
            } else {
                window.draw(right);
            }
			timer_left_key = clock();
        } else {
            window.draw(up);
        }
		} else if (key_state == 4) {
		if ((clock() - std::max(timer_left_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
			if (!is_left_handed) {
				window.draw(right);
			}
			else {
				window.draw(left);
			}
			timer_right_key = clock();
		}
		else {
			window.draw(up);
		}
		} else if (key_state == 5) {
		if ((clock() - std::max(timer_right_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
			if (!is_left_handed) {
				window.draw(left);
			}
			else {
				window.draw(right);
			}
			timer_left_key = clock();
		}
		else {
			window.draw(up);
		}
		} else if (key_state == 6) {
		if ((clock() - std::max(timer_left_key, timer_wave_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
			if (!is_left_handed) {
				window.draw(right);
			}
			else {
				window.draw(left);
			}
			timer_right_key = clock();
		}
		else {
			window.draw(up);
		}
		} else if (key_state == 7) {
			if ((clock() - std::max(timer_left_key, timer_right_key)) / CLOCKS_PER_SEC > BONGO_KEYPRESS_THRESHOLD) {
				window.draw(wave);
				timer_wave_key = clock();
			} else {
				window.draw(up);
			}
		}

    // drawing tablet
    if (!is_mouse) {
        window.draw(device);
    }
    
    // drawing smoke
    for (Json::Value &v : smoke_key_value) {
        if (GetKeyState(VK_SCROLL) & 0x0001) {
            window.draw(smoke);
            break;
        }
    }
    
    // drawing sunglass and mirrorball
    sunglass_mirrorball(is_reverse);

}
}; // namespace osu
