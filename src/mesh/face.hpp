#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_ds_face_base_2.h>

#include <CGAL/Triangulation_face_base_2.h>

#include <set>
#include <string>

#include <boost/shared_ptr.hpp>

#include <boost/ptr_container/ptr_map.hpp>
#include "timeseries.hpp"

/**
* \struct face_info
* A way of embedding arbirtrary data into the face. This is how modules should store their data.
*/
struct face_info
{

    virtual ~face_info()
    {
    };
};



typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef K::Vector_3 Vector_3;
typedef K::Triangle_3 Triangle_3;

/**
* \class face
* \brief Defines the triangle face
*/
template < class Gt, class Fb = CGAL::Triangulation_face_base_2<Gt> >
class face
: public Fb
{
public:

//    face_info* info;

    /**
    * \typedef Vertex_handle Handle to a vertex
    */
    typedef typename Fb::Vertex_handle Vertex_handle;

    /**
    * \typedef Face_handle Handle to a face
    */
    typedef typename Fb::Face_handle Face_handle;


    template < typename TDS2 >
    struct Rebind_TDS
    {
        typedef typename Fb::template Rebind_TDS<TDS2>::Other Fb2;
        typedef face<Gt, Fb2> Other;
    };

    face();
    face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2);
    face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Face_handle n0, Face_handle n1, Face_handle n2);
    face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2, Face_handle n0, Face_handle n1, Face_handle n2, bool c0, bool c1, bool c2);

    ~face();

    /**
    * Aspect of the face. North = 0, CW . Calculated on first use, subsequent usages will not recalculate
    * \return Face aspect [rad]
    */
    double aspect();

    /**
    * Slope of the face. Calculated on first use, subsequent usages will not recalculate
    * \return slope [rad]
    */
    double slope();

    /**
    * Normalized face normal. Calculated on first use, subsequent usages will not recalculate
    */
    Vector_3 normal();

    /**
    * Center of the face as defined by a centroid. Calculated on first use, subsequent usages will not recalculate
    */
    Point_3 center();

    /**
    * Checks if a point x,y is within the face
    * \return true if this face contains the point x,y
    */
    bool contains(double x, double y);

    /**
    * Checks if a point p is within the face
    * \return true if this face contains the point p
    */
    bool contains(Point_3 p);

    /**
    * Checks if the given face intersects this face in 2D. The algorithm checks if the specified face contains any of the 3 vertexs or center of this face.
    * \param fh Face handle to the other face
    * \return true if this face and fh intersects
    */
    bool intersects(Face_handle fh);

    /**
    * Sets the face data, for the given variable, at the current timestep
    * \param variable Variable to assign the data to
    * \param data Data
    */
    void set_face_data(std::string variable, double data);

    /**
    * Returns the face data
    * \param variable  variable being queried
    * \return  this timesteps value for the given variable
    */
    double face_data(std::string variable);

    /**
    * Initializes  this faces timeseires with the given varialves, for the given datetime series with the given size
    * \param variables Names of the variables to add
    * \param datetime Vector of boost::ptimes for the entire duration of the timesries
    */
    void init_time_series(std::set<std::string> variables, timeseries::date_vec datetime);

    /**
    * Obtains the timeseries associated with the given variable
    * \param ID variable
    */
    timeseries::variable_vec face_time_series(std::string ID);

    /**
     * Returns a list of variables in this face's timeseries
     * \return Vector of variable names
     */
    std::vector<std::string> variables();

    /**
    * Returns the underlying timeseries object
    * \return Pointer to the underlying timeseries
    */
    boost::shared_ptr<timeseries> get_underlying_timeseries();

    /**
    * Returns the iterator of the current timestep.
    */
    timeseries::iterator now();

    /**
     * Increments the face to the next timestep
     */
    void next();
    
    /**
     * Resets the face to the start of the timeseries
     */
    void reset_to_begining();
    
    /**
     * Returns the xcoordinates of the center of the face
     * @return x UTM coordinate
     */
    double get_x();
    
    /**
     * Returns the ycoordinates of the center of the face
     * @return y UTM coordinate
     */
    double get_y();
    
    /**
     * Returns the zcoordinates of the center of the face
     * @return  elevation
     */
    double get_z();
    
    /**
     * Saves this face's timeseries to a file
     * @param fname specified file name
     */
    void to_file(std::string fname);

    face_info* module_face_data(std::string module);
    void set_module_face_data(std::string module,face_info* fi);
private:

    double _slope;
    double _azimuth;
    double _x;
    double _y;
    double _z;
    boost::shared_ptr<Point_3> _center;
    boost::shared_ptr<Vector_3> _normal;

    //boost::ptr_map<std::string,face_info> _module_face_data;
    std::map<std::string,face_info* > _module_face_data;
    boost::shared_ptr<timeseries> _data;
    timeseries::iterator _itr;


};

template < class Gt, class Fb >
face<Gt, Fb>::~face()
{
    for(auto itr : _module_face_data)
    {
        delete itr.second;
    }
};
template < class Gt, class Fb >
face<Gt, Fb>::face()
{
    _slope = -1;
    _azimuth = -1;
    _data = boost::make_shared<timeseries>();

    _center = NULL;
    _normal = NULL;




}

template < class Gt, class Fb>
face<Gt, Fb>::face(Vertex_handle v0,
        Vertex_handle v1,
        Vertex_handle v2)
: Fb(v0, v1, v2)
{
    _slope = -1;
    _azimuth = -1;
    _data = boost::make_shared<timeseries>();

}

template < class Gt, class Fb >
face<Gt, Fb>::face(Vertex_handle v0,
        Vertex_handle v1,
        Vertex_handle v2,
        Face_handle n0,
        Face_handle n1,
        Face_handle n2)
: Fb(v0, v1, v2, n0, n1, n2)
{
    _slope = -1;
    _azimuth = -1;
    _data = boost::make_shared<timeseries>();


}

template < class Gt, class Fb >
face<Gt, Fb>::face(Vertex_handle v0,
        Vertex_handle v1,
        Vertex_handle v2,
        Face_handle n0,
        Face_handle n1,
        Face_handle n2,
        bool c0,
        bool c1,
        bool c2)
: Fb(v0, v1, v2, n0, n1, n2)
{
    _slope = -1;
    _azimuth = -1;
    _data = boost::make_shared<timeseries>();


}

template < class Gt, class Fb>
double face<Gt, Fb>::aspect()
{
    if (_azimuth == -1)
    {
        if(!_normal)
            this->normal();
        //convert normal to spherical
//        double r = sqrt(_normal[0] * _normal[0] + _normal[1] * _normal[1] + _normal[2] * _normal[2]);
//        double theta = acos(_normal[2] / r);

        //y=north
        double phi = atan2((*_normal)[1], (*_normal)[0]); // + M_PI /*-3.14159/2*/; //south == 0
        _azimuth = phi - M_PI / 2.0; //set north = 0

        if (_azimuth < 0.0)
            _azimuth += 2.0 * M_PI;
    }

    return _azimuth;
}

template < class Gt, class Fb>
double face<Gt, Fb>::slope()
{
    if (_slope == -1)
    {
        if(!_normal)
            this->normal();

        //z surface normal
        arma::vec n(3);
        n(0) = 0.0; //x
        n(1) = 0.0; //y
        n(2) = 1.0;

        arma::vec normal(3);
        normal(0) = (*_normal)[0];
        normal(1) = (*_normal)[1];
        normal(2) = (*_normal)[2];

        _slope = acos(arma::norm_dot(normal, n));
    }

    return _slope;
}

template < class Gt, class Fb>
bool face<Gt, Fb>::intersects(face<Gt, Fb>::Face_handle fh)
{
    //does t contain my points?
    bool intersect = fh->contains(this->center());
    if (!intersect)
    {
        if (fh->contains(this->vertex(0)->point()) ||
                fh->contains(this->vertex(1)->point()) ||
                fh->contains(this->vertex(2)->point()))
        {
            intersect = true;
        }
    }
    return intersect;
}

template < class Gt, class Fb>
Vector_3 face<Gt, Fb>::normal()
{
    if(!_normal)
        _normal = boost::make_shared<Vector_3>(CGAL::unit_normal(this->vertex(0)->point(), this->vertex(1)->point(), this->vertex(2)->point()));
    return *_normal;
}

template < class Gt, class Fb>
Point_3 face<Gt, Fb>::center()
{
    if (!_center)
    {
        _center = boost::make_shared<Point_3>(CGAL::centroid(this->vertex(0)->point(), this->vertex(1)->point(), this->vertex(2)->point()));
        _x=_center->x();
        _y=_center->y();
        _z=_center->z();
    }
    //return CGAL::centroid(this->vertex(0)->point(), this->vertex(1)->point(), this->vertex(2)->point());
    return *_center;

}
template < class Gt, class Fb>
bool face<Gt, Fb>::contains(Point_3 p)
{
    return this->contains(p.x(), p.y());
}

template < class Gt, class Fb>
bool face<Gt, Fb>::contains(double x, double y)
{
    
    double x1 = this->vertex(1)->point().x();
    double y1 = this->vertex(1)->point().y();

    double x2 = this->vertex(2)->point().x();
    double y2 = this->vertex(2)->point().y();

    double x3 = this->vertex(0)->point().x();
    double y3 = this->vertex(0)->point().y();

    double lambda1 = ((y2 - y3)*(x - x3)+(x3 - x2)*(y - y3)) / ((y2 - y3)*(x1 - x3)+(x3 - x2)*(y1 - y3));

    if( !(lambda1 > 0.0 && lambda1 < 1.0) )
        return false; //bail early if possible

    double lambda2 = ((y3 - y1)*(x - x3)+(x1 - x3)*(y - y3)) / ((y3 - y1)*(x2 - x3)+(x1 - x3)*(y2 - y3));
    double lambda3 = 1.0 - lambda1 - lambda2;

    return lambda1 > 0.0 && lambda1 < 1.0
            && lambda2 > 0.0 && lambda2 < 1.0
            && lambda3 > 0.0 && lambda3 < 1.0;

}
template < class Gt, class Fb>
std::vector<std::string> face<Gt, Fb>::variables()
{
    return _data->list_variables();
}
template < class Gt, class Fb>
void face<Gt, Fb>::set_face_data(std::string variable, double data)
{
    _itr->set(variable, data);
}

template < class Gt, class Fb>
double face<Gt, Fb>::face_data(std::string variable)
{
    return _itr->get(variable);
}

template < class Gt, class Fb>
void face<Gt, Fb>::init_time_series(std::set<std::string> variables, timeseries::date_vec datetime)
{
    _data->init(variables, datetime);

    _itr = _data->begin();
}

template < class Gt, class Fb>
timeseries::variable_vec face<Gt, Fb>::face_time_series(std::string ID)
{
    return _data->get_time_series(ID);
}

template < class Gt, class Fb>
void face<Gt, Fb>::next()
{
    ++_itr;
}

template < class Gt, class Fb>
void face<Gt, Fb>::reset_to_begining()
{
    _itr = _data->begin();
}

template < class Gt, class Fb>
void face<Gt, Fb>::to_file(std::string fname)
{
    _data->to_file(fname);
}

template < class Gt, class Fb>
double face<Gt, Fb>::get_x()
{
    if(!_center)
        this->center();

    return _x;
}

template < class Gt, class Fb>
double face<Gt, Fb>::get_y()
{
    if(!_center)
        this->center();


    return _y;
}

template < class Gt, class Fb>
double face<Gt, Fb>::get_z()
{
    if(!_center)
        this->center();

    return _z;
}
template < class Gt, class Fb>
boost::shared_ptr<timeseries> face<Gt, Fb>::get_underlying_timeseries()
{
    return _data;
}

template < class Gt, class Fb>
timeseries::iterator face<Gt, Fb>::now()
{
    return _itr;
}


template < class Gt, class Fb>
face_info* face<Gt, Fb>::module_face_data(std::string module)
{
    face_info* info=nullptr;
    try
    {
        info = _module_face_data[module];
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(module_data_error() << errstr_info ("No data for module " + module));
    }

    return info;


}

template < class Gt, class Fb>
void face<Gt, Fb>::set_module_face_data(std::string module,face_info* fi)
{
    _module_face_data[module] = fi;
}