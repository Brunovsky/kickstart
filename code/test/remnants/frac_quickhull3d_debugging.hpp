// Code to debug frac_quickhull3d

#ifdef DEBUG_HULL3D
#define my_ass(test, msg) assert(test&& bool(msg))

    void debug_iteration(int iteration, int eye = -1) {
        print("===== iteration {}, nfaces={}\n", iteration, faces.size());
        auto ordered_faces = extract_hull(0);

        if (eye != -1) {
            int id = eye_face[eye]->id;
            print("eye={} -> {}    face={} -> [{}]\n", eye - 1, points[eye], id,
                  ordered_faces[id]);
        }
        debug();
    }

    void debug_eyes() {
        print("      nodes: NIL");
        for (int v = 1; v <= N; v++) {
            print(" {:>2}", v - 1);
        }
        cout << endl;

        print("   eye_face: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_face[v] ? to_string(eye_face[v]->id) : "");
        }
        cout << endl;

        print("   eye_next: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_next[v] ? to_string(eye_next[v] - 1) : "");
        }
        cout << endl;

        print("   eye_prev: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", eye_prev[v] ? to_string(eye_prev[v] - 1) : "");
        }
        cout << endl;

        print("       open: ");
        for (int v = 0; v <= N; v++) {
            print(" {:>2}", open[v] ? to_string(open[v] - 1) : "");
        }
        cout << endl;

        auto ordered_faces = extract_hull(0);

        print("new faces:");
        for (int f = 0; f < int(new_faces.size()); f++) {
            print(" {}=[{}]", new_faces[f]->id, ordered_faces[new_faces[f]->id]);
        }
        print("\n");

        print("outsides:");
        for (int f = 0; f < int(faces.size()); f++) {
            print("  {}->{}", f, faces[f]->outside);
        }
        cout << endl;
    }

    void debug_faces() {
        auto ordered_faces = extract_hull(0);

        print("faces:");
        for (int i = 0, f = ordered_faces.size(); i < f; i++) {
            print(" {}=[{}]", i, ordered_faces[i]);
        }
        cout << endl;
    }

    void debug_horizon() {
        print("horizon:\n");
        for (Edge* edge : horizon) {
            assert(edge && edge->face->mark == DELETED);
            print("   {} (face={})\n", edge->vertex - 1, edge->face->id);
        }
        cout << flush;
    }

    void debug() {
        debug_faces();
        debug_eyes();
        debug_horizon();
    }

    void verify_horizon() {
        unordered_set<Edge*> horizonset(begin(horizon), end(horizon));
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));

        for (Edge* edge : horizon) {
            Face *face = edge->face, *oface = edge->opposite->face;
            my_ass(face->mark == DELETED && oldset.count(face),
                   "horizon edge is apparently visible");
            my_ass(oface->mark == VISIBLE && !oldset.count(oface),
                   "horizon opposite edge is apparently not visible");
        }
    }

    void verify_active_faces() {
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));
        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();
            my_ass(face, "faces[] contains null list");
            my_ass(face->edge, "face has null head edge");
            my_ass(face->edge->face == face, "face head edge assigned to wrong face");
            my_ass(faces[face->id].get() == face, "face in wrong position in faces[]");
            my_ass((face->mark == VISIBLE || oldset.count(face)), "not in old_faces");
            my_ass(face->outside >= 0 && face->outside <= N, "bad face outside eye");
        }
    }

    void verify_old_faces() {
        unordered_set<Face*> oldset(begin(old_faces), end(old_faces));

        for (Face* face : old_faces) {
            my_ass(face->mark == DELETED, "old face is not deleted");
        }
    }

    void verify_eyes() {
        vector<unordered_set<int>> eyes(faces.size());

        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();
            int eye = face->outside;
            while (eye && eye_face[eye] == face) {
                eyes[face->id].insert(eye);
                eye = eye_next[eye];
            }
        }

        for (int v = 1; v <= N; v++) {
            if (eye_face[v] == nullptr) {
                my_ass(eye_prev[v] == 0 && eye_next[v] == 0, "eye was not cleared");
                continue;
            }
            Face* face = eye_face[v];
            my_ass(face && faces[face->id].get() == face, "bad eye face");
            my_ass(face && face->mark == VISIBLE, "bad eye face");

            my_ass(eyes[face->id].count(v), "eye not reachable from face");
            my_ass(eye_prev[eye_next[v]] == v, "fucked up eye list next");
            my_ass(eye_next[eye_prev[v]] == v, "fucked up eye list prev");
        }
    }

    void verify_edges(bool nocoplanar = false) {
        for (int f = 0, Q = faces.size(); f < Q; f++) {
            Face* face = faces[f].get();

            vector<Edge*> edges;
            Edge* edge = face->edge;
            do {
                my_ass(edge->face == face, "edge assigned to wrong face");
                my_ass(edge->prev->next == edge, "edge incorrect prev->next");
                my_ass(edge->next->prev == edge, "edge incorrect next->prev");
                my_ass(edge->opposite->opposite == edge, "edge incorrect opposite");
                my_ass(edge->opposite->face != face, "edge opposite shares face");
                my_ass(edge->next->vertex == edge->opposite->vertex,
                       "incorrect next->vertex/opposite->vertex");

                int v = edge->vertex;
                my_ass(face->plane.planeside(points[v]) == 0, "vertex is not in plane");
                my_ass(edge->opposite->face->plane.planeside(points[v]) == 0,
                       "vertex is not in opposite face's plane");

                if (nocoplanar) {
                    my_ass(face->plane != edge->opposite->face->plane,
                           "should have merged faces");
                }

                edges.push_back(edge);
                edge = edge->next;
            } while (edge != face->edge);
        }
    }

    void verify_invariants() {
        verify_horizon();
        verify_active_faces();
        verify_old_faces();
        verify_eyes();
        verify_edges(true);
    }
#endif