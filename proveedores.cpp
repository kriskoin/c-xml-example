/*
para compilar
 g++ -Wall proveedores.cpp tinyxml2.cpp -o proveedores
 
 como usar
 ./proveedores facturas 2022 04 
 para generar un reporte basado en las facturas emitidas
 para el mes de 04 del 2022,nombre del archivo  reporte_facturas_2021_04.csv

 ./proveedores confirmaciones 2022 04 para respuestas

para generar un reporte basado en las confirmaciones de hacienda
 para el mes de 04 del 2021 , nombre del archivo reporte_confirmaciones_2021_04.csv

 
*/
#include "tinyxml2.h"
#include <iostream>
#include <iomanip> 
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <limits>
#include <math.h>

using namespace std;
using namespace tinyxml2;

namespace my {

    std::string to_string( double d ) {

        std::ostringstream stm ;
        stm << std::setprecision(std::numeric_limits<double>::digits10) << d ;
        return stm.str() ;
    }
}

;


typedef struct fact_data
{
    string clave="NO";
    string numConsecutivo="NO";
    string fecha;
    double TotalMercanciasGravadas = 0; 
    double TotalMercanciasExentas = 0;
    double TotalMercExonerada = 0;
    double TotalGravado = 0;
    double TotalExento = 0;
    double TotalExonerado = 0;
    double TotalVenta = 0;
    double TotalDescuentos = 0;
    double TotalVentaNeta = 0;
    double TotalImpuesto = 0;
    double TotalIVADevuelto = 0;
    double TotalComprobante = 0;
}FAC_DATA;

typedef struct prov_data
{
    string name;
  	double total_facturas = 0;
    double total_impuestos = 0; 
    int numDocs = 0;
    std::list<FAC_DATA> facturas;
}PROV_DATA;

std::list<std::string> errorsLog;

std::list<std::string> listFiles;
std::set<std::string> cedulaSet;
std::map<std::string,PROV_DATA> listProveedores;
std::list<FAC_DATA> listFacturas;
bool flagFacturas =false;
bool flagRespuestas = false;
std::string year;
std::string month;
static void print_report( 
    std::string filename,
    std::map<std::string,PROV_DATA> * prov
){
    cout<< "############ REPORT ############" <<endl;
    //if ((int)prov->size()){
        std::map<std::string,PROV_DATA>::iterator it;
        for ( it=prov->begin(); it!=prov->end(); ++it){
            std::cout <<"cedula: "<< it->first << " nombre: " << it->second.name <<" total fact: "<<std::setprecision(9)<<it->second.total_facturas<<" total imp:"<< std::setprecision(9)<<it->second.total_impuestos<<" numDocs: "<<it->second.numDocs<< endl;
        }
  //  }
    cout<< "############ END REPORT ############" <<endl;
}


std::string getReportBlankLine (){
    std::string str;
    str.assign("  ; ; ; ; ; ; ; ; ; ; \n");
    return str;
}

std::string getReportLineHeader (){
    std::string str;
    str.assign("Nombre Proveedor ; Cedula ; Total Factura ;  Total Impuesto ; numDocs ; ");
    str.append("Fecha ; ");
    str.append("Consecutivo ; ");
    str.append("Total ; ");
    str.append("TotalImpuesto ; ");
     str.append(" % ; ");
    str.append("clave ; ");
    str.append("\n");

    /*  string clave;
    string numConsecutivo; *
    string fecha; *
    double TotalMercanciasGravadas = 0; 
    double TotalMercanciasExentas = 0;
    double TotalMercExonerada = 0;
    double TotalGravado = 0;
    double TotalExento = 0;
    double TotalExonerado = 0;
    double TotalVenta = 0;
    double TotalDescuentos = 0;
    double TotalVentaNeta = 0;
    double TotalImpuesto = 0; *
    double TotalIVADevuelto = 0; 
    double TotalComprobante = 0; * 
    */
    
    return str;
}

std::string getReportProveedorDetail (std::string key,PROV_DATA * pData){
    std::string str;
    str.assign(pData->name.c_str());
    str.append(" ; ");
    str.append(key);
    str.append(" ; ");
    str.append(std::to_string(pData->total_facturas));
    str.append(" ; ");
    str.append(std::to_string(pData->total_impuestos));
    str.append(" ; ");
    str.append(std::to_string(pData->numDocs));
    ///detalle
    str.append("\n");
    return str;
}


static void writeListaFacturas(
    FILE * pFile,
    std::list<FAC_DATA> * facturas
){
    std:string str;
    std::list<FAC_DATA>::iterator it;
    double tmp;
    for ( it=facturas->begin(); it!=facturas->end(); ++it){
        str.assign(" ;  ;  ;  ; ; ");
        str.append(it->fecha);
        str.append(" ; ");
        str.append(it->numConsecutivo);
        str.append(" ; ");
        str.append(std::to_string(it->TotalComprobante));
        str.append(" ; ");
        str.append(std::to_string(it->TotalImpuesto));
        str.append(" ; ");
        // porcentaje
        if(it->TotalImpuesto){
            tmp = (it->TotalComprobante - it->TotalImpuesto);
            tmp = it->TotalImpuesto/tmp;
            tmp = tmp * 100;
            //tmp = round(tmp) ;
        }else{
            tmp = 0;
        }
        str.append(std::to_string(tmp));
        str.append(" ; ");
        str.append(it->clave);
        str.append("\n");
        fputs (str.c_str(),pFile);
        fflush(pFile);
    }

};
static void create_report_detalle( 
    std::string filename,
    std::map<std::string,PROV_DATA> * prov
){
    /*
    
    */

    if ((int)prov->size()){
        FILE * pFile;
        string str;
        pFile = fopen (filename.c_str(),"w");
        str.assign(getReportLineHeader());
        fputs (str.c_str(),pFile);
        fflush(pFile);
        std::map<std::string,PROV_DATA>::iterator it;
        for ( it=prov->begin(); it!=prov->end(); ++it){
            str.assign(getReportProveedorDetail(it->first.c_str(),&it->second));
            fputs (str.c_str(),pFile);
            writeListaFacturas(pFile,&(it->second.facturas));
            str.assign(getReportBlankLine());
            fputs (str.c_str(),pFile);
            fflush(pFile);
        }
        fclose (pFile);
    }
    
}

static void create_report( 
    std::string filename,
    std::map<std::string,PROV_DATA> * prov
){
    
    if ((int)prov->size()){
        FILE * pFile;
        string str;
        pFile = fopen (filename.c_str(),"w");
        
        str.assign("Nombre Proveedor ; Cedula ; Total Factura ;  Total Impuesto ; numDocs \n");
        fputs (str.c_str(),pFile);
        fflush(pFile);
        std::map<std::string,PROV_DATA>::iterator it;
        for ( it=prov->begin(); it!=prov->end(); ++it){
            str.assign(it->second.name.c_str());
            str.append(" ; ");
            str.append(it->first.c_str());
            str.append(" ; ");
            str.append(std::to_string(it->second.total_facturas));
            str.append(" ; ");
            str.append(std::to_string(it->second.total_impuestos));
            str.append(" ; ");
            str.append(std::to_string(it->second.numDocs));
            str.append("\n");
            fputs (str.c_str(),pFile);
            fflush(pFile);
        }
        fclose (pFile);
    }
    
}


static void create_report_detallado( 
    std::string filename,
    std::map<std::string,PROV_DATA> * prov
){
    
    if ((int)prov->size()){
        FILE * pFile;
        string str;
        pFile = fopen (filename.c_str(),"w");
        
        str.assign("Nombre Proveedor ; Cedula ; Total Factura ;  Total Impuesto ; numDocs ;");
        //str.append("Nombre Proveedor ; Cedula ; Total Factura ;  Total Impuesto ; numDocs ;");

        fputs (str.c_str(),pFile);
        fflush(pFile);
        std::map<std::string,PROV_DATA>::iterator it;
        for ( it=prov->begin(); it!=prov->end(); ++it){
            str.assign(it->second.name.c_str());
            str.append(" ; ");
            str.append(it->first.c_str());
            str.append(" ; ");
            str.append(std::to_string(it->second.total_facturas));
            str.append(" ; ");
            str.append(std::to_string(it->second.total_impuestos));
            str.append(" ; ");
            str.append(std::to_string(it->second.numDocs));
            str.append("\n");
            fputs (str.c_str(),pFile);
            fflush(pFile);
        }
        fclose (pFile);
    }
    
}



bool valid_name(char * strName){
    int size = (unsigned)strlen(strName);
    if (size < 4) return false;
    
    if ((strName[size-1]=='l') &&
        (strName[size-2]=='m') &&
        (strName[size-3]=='x') &&
        (strName[size-4]=='.') 
      ){
        return true ;
    }
    return false;
}

static void list_dir(const char *path, std::list<std::string> * listFiles) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if(valid_name(entry->d_name)){
            listFiles->push_back(entry->d_name);
        }
    }
    closedir(dir);
}


bool extract_data_factura_msg(
    XMLElement * xmlData, 
    PROV_DATA * data
    ) {
        
        data->total_facturas = 0;
        data->total_impuestos = 0;
        //NombreEmisor
        XMLElement * nombreEmisor = xmlData->FirstChildElement("Emisor")->FirstChildElement("Nombre");
        if (NULL == nombreEmisor) {
            return false;
        }
        data->name.assign(nombreEmisor->GetText());

        //MontoTotalImpuesto
        XMLElement * totalImpuesto = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalImpuesto");    
        if (NULL == totalImpuesto) {
            return false;
        }
        totalImpuesto->QueryDoubleText( &data->total_impuestos );

        //TotalFactura
        XMLElement * totalFactura = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalComprobante");
        if (NULL == totalFactura) {
            return false;
        }
        
       totalFactura->QueryDoubleText( &data->total_facturas);
        return true;
    }


    bool extract_data_detalle_factura_msg(
    XMLElement * xmlData, 
    FAC_DATA * data
    ) {
        
        data->TotalComprobante = 0;
        data->TotalDescuentos = 0;
        data->TotalGravado = 0;
        data->TotalImpuesto = 0;
        data->TotalIVADevuelto = 0;
        data->TotalMercanciasExentas = 0;
        data->TotalMercanciasGravadas = 0;
        data->TotalMercExonerada = 0 ;
        data->TotalVenta = 0;
        data->TotalVentaNeta = 0;

        //clave del documento
        XMLElement * xmlClave = xmlData->FirstChildElement("Clave");
        if (NULL == xmlClave) {
            return false;
        }
        data->clave.assign(xmlClave->GetText());

        //numero consecutivo
        XMLElement * xmlConsecutivo = xmlData->FirstChildElement("NumeroConsecutivo");
        if (NULL == xmlConsecutivo) {
            return false;
        }
        data->numConsecutivo.assign(xmlConsecutivo->GetText());

        //fecha documento
        XMLElement * xmlFecha = xmlData->FirstChildElement("FechaEmision");
         if (NULL == xmlFecha) {
            return false;
        }
        data->fecha.assign(xmlFecha->GetText());
/*
        XMLElement * xmlMercaGravada = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalMercanciasGravadas");
         if (NULL == xmlMercaGravada) {
            return false;
        }
        xmlMercaGravada->QueryDoubleText( &data->TotalMercanciasGravadas );

        XMLElement * xmlMercaExent = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalMercanciasExentas");
         if (NULL == xmlMercaExent) {
            return false;
        }
        xmlMercaExent->QueryDoubleText( &data->TotalMercanciasExentas );

        
        XMLElement * xmlMercaExone = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalMercExonerada");
         if (NULL == xmlMercaExone) {
            return false;
        }
        xmlMercaExone->QueryDoubleText( &data->TotalMercExonerada);

        XMLElement * xmlTotalGravado = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalGravado");
        if (NULL == xmlTotalGravado) {
            return false;
        }
        xmlTotalGravado->QueryDoubleText( &data->TotalGravado);

        XMLElement *  xmlTotalExento = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalExento");
        if (NULL == xmlTotalExento) {
            return false;
        }
        xmlTotalExento->QueryDoubleText( &data->TotalExento);

        XMLElement *  xmlTotalExonerado = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalExonerado");
        if (NULL == xmlTotalExonerado) {
            return false;
        }
        xmlTotalExonerado->QueryDoubleText( &data->TotalExonerado);

        XMLElement *  xmlTotalVenta = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalVenta");
        if (NULL == xmlTotalVenta) {
            return false;
        }
        xmlTotalVenta->QueryDoubleText( &data->TotalVenta);

        XMLElement *  xmlTotalDescuentos = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalDescuentos");
        if (NULL == xmlTotalDescuentos) {
            return false;
        }
        xmlTotalDescuentos->QueryDoubleText( &data->TotalDescuentos);

        XMLElement *  xmlTotalVentaNeta = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalVentaNeta");
        if (NULL == xmlTotalVentaNeta) {
            return false;
        }
        xmlTotalVentaNeta->QueryDoubleText( &data->TotalVentaNeta);
*/

        XMLElement *  xmlTotalImpuesto = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalImpuesto");
        if (NULL == xmlTotalImpuesto) {
            return false;
        }
        xmlTotalImpuesto->QueryDoubleText( &data->TotalImpuesto);
/*
        XMLElement *  xmlTotalIVADevuelto = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalIVADevuelto");
        if (NULL == xmlTotalIVADevuelto) {
            return false;
        }
        xmlTotalIVADevuelto->QueryDoubleText( &data->TotalIVADevuelto);
*/
        XMLElement *  xmlTotalComprobante = xmlData->FirstChildElement("ResumenFactura")->FirstChildElement("TotalComprobante");
        if (NULL == xmlTotalComprobante) {
            return false;

        }
        xmlTotalComprobante->QueryDoubleText( &data->TotalComprobante);

        return true;
    }

    bool extract_data_hacienda_msg(
    XMLElement * xmlData, 
    PROV_DATA * data
    ) {
        data->total_facturas = 0;
        data->total_impuestos = 0;
        //NombreEmisor
        XMLElement * nombreEmisor = xmlData->FirstChildElement("NombreEmisor");
        data->name.assign(nombreEmisor->GetText());

        //MontoTotalImpuesto
        XMLElement * totalImpuesto = xmlData->FirstChildElement("MontoTotalImpuesto");    
        if (NULL == totalImpuesto) {
            return false;
        }
        totalImpuesto->QueryDoubleText( &data->total_impuestos );

        //TotalFactura
        XMLElement * totalFactura = xmlData->FirstChildElement("TotalFactura");
        if (NULL == totalFactura) {
            return false;
        }
        
       totalFactura->QueryDoubleText( &data->total_facturas);
        return true;
    }



static void process_factura_msg(
    XMLElement * facturaElectronica, 
    std::set<std::string> * cedulas,
    std::set<std::string> * claves,
    std::map<std::string,PROV_DATA> * prov,
    std::list<std::string> * errsLog
    ) {
         
        XMLElement * clave = facturaElectronica->FirstChildElement("Clave");
        if (NULL != clave) {
            
            std::set<std::string>::iterator it = claves->find(clave->GetText());
            if(it == claves->end()){
                
                //clave no ha sido procesada
                //verifica la cedula del receptor
                XMLElement * receptorId = facturaElectronica->FirstChildElement("Emisor")->FirstChildElement("Identificacion")->FirstChildElement("Numero");
                if (NULL != receptorId) {
                    
                    std::set<std::string>::iterator itCedula = cedulas->find(receptorId->GetText());
                    if(itCedula != cedulas->end()){

                        
                        //cedula receptor ok

                        PROV_DATA pdata;
                        FAC_DATA detalleData;

                        //fecha emision
                        XMLElement * fechaEmision = facturaElectronica->FirstChildElement("FechaEmision");
                        std::string fechaStr;                        

                        fechaStr.assign(fechaEmision->GetText());//2021-04-23T11:15:12-06:00
                        
                        if (
                            (std::string(fechaStr.substr(0,4)).compare(year) == 0) &&
                            (std::string(fechaStr.substr(5,2)).compare(month) == 0) 
                        ){

  
                            //cedula emisor
                            XMLElement * cedulaEmisor = facturaElectronica->FirstChildElement("Emisor")->FirstChildElement("Identificacion")->FirstChildElement("Numero");
                            std::map<std::string,PROV_DATA>::iterator itCedulaEmisor = prov->find(cedulaEmisor->GetText());

                            bool dataOK = extract_data_factura_msg(facturaElectronica,&pdata);
                            //bool dataOK = true;
                            bool dataDetalle = extract_data_detalle_factura_msg(facturaElectronica,&detalleData);
                            //bool dataDetalle = true;
                            //bool dataOK = true;
                            if( dataOK && dataDetalle ){
                                if(itCedulaEmisor==prov->end()){
                                    //inserta el proveedor
                                    pdata.numDocs=1;
                                    pdata.facturas.push_back(detalleData);
                                    prov->insert ( std::pair<std::string,PROV_DATA>(cedulaEmisor->GetText(),pdata) );
                                    
                                }else{
                                    //suma el proveedor
                                    itCedulaEmisor->second.facturas.push_back(detalleData);
                                    itCedulaEmisor->second.numDocs++;
                                    itCedulaEmisor->second.total_facturas = itCedulaEmisor->second.total_facturas + pdata.total_facturas;
                                    itCedulaEmisor->second.total_impuestos = itCedulaEmisor->second.total_impuestos + pdata.total_impuestos;
                                }

                            }else{
                                string strTmp;
                                strTmp.append("Clave: ");
                                strTmp.append(clave->GetText());
                                strTmp.append(" Sin datos de faturacion");
                                errsLog->push_back(strTmp);
                            }
                        } //verifica fecha
                        }else{
                            //cedula no listada como trusted
                           // errsLog->push_back(std::string(receptorId->GetText()).append(" Cedula no trusted on clave ").append(clave->GetText()));     
                    } //cedula no existe                  
                } 
                //agrega la clave como procesada
                claves->insert(clave->GetText());
            }else{
                errsLog->push_back(std::string(clave->GetText()).append(" WARN respuesta dup "));
            } //if(it == claves->end())
        }
        //std::cout << " factura ";
}

static void process_hacienda_msg(
    XMLElement * hacienda,
    std::set<std::string> * cedulas,
    std::set<std::string> * claves,
    std::map<std::string,PROV_DATA> * prov,
    std::list<std::string> * errsLog
    ) {
    XMLElement * clave = hacienda->FirstChildElement("Clave");
        if (NULL != clave) {
            
            std::set<std::string>::iterator it = claves->find(clave->GetText());
            if(it == claves->end()){
                
                //clave no ha sido procesada
                //verifica la cedula del receptor
                XMLElement * cedulaReceptor = hacienda->FirstChildElement("NumeroCedulaReceptor");
                if (NULL != cedulaReceptor) {
                    
                    std::set<std::string>::iterator itCedula = cedulas->find(cedulaReceptor->GetText());
                    if(itCedula != cedulas->end()){
                        
                        //cedula receptor ok
                        std::string fechaStr;
                        PROV_DATA pdata;
                        FAC_DATA detalleData; 

                        XMLElement * fechaFirmado = hacienda->FirstChildElement("ds:Signature")->FirstChildElement("ds:Object")->FirstChildElement("xades:QualifyingProperties")->FirstChildElement("xades:SignedProperties")->FirstChildElement("xades:SignedSignatureProperties")->FirstChildElement("xades:SigningTime");
                        
                        fechaStr.assign(fechaFirmado->GetText());//2021-04-05T16:54:46Z
                        
                        if (
                            (std::string(fechaStr.substr(0,4)).compare(year) == 0) &&
                            (std::string(fechaStr.substr(5,2)).compare(month) == 0) 
                        ){


                            //cedula emisor
                            XMLElement * cedulaEmisor = hacienda->FirstChildElement("NumeroCedulaEmisor");
                            std::map<std::string,PROV_DATA>::iterator itCedulaEmisor = prov->find(cedulaEmisor->GetText());

        
                            bool dataOK = extract_data_hacienda_msg(hacienda,&pdata);
                            
                            if(dataOK){
                                detalleData.fecha.assign(fechaStr);
                                detalleData.clave.assign(clave->GetText());
                                detalleData.TotalComprobante = pdata.total_facturas;
                                detalleData.TotalImpuesto = pdata.total_impuestos;
                               
                                if(itCedulaEmisor==prov->end()){
                                    //inserta el proveedor
                                    pdata.numDocs=1;
                                    pdata.facturas.push_back(detalleData);
                                    prov->insert ( std::pair<std::string,PROV_DATA>(cedulaEmisor->GetText(),pdata) );
                                    
                                }else{
                                    //suma el proveedor
                                    itCedulaEmisor->second.facturas.push_back(detalleData);
                                    itCedulaEmisor->second.numDocs++;
                                    itCedulaEmisor->second.total_facturas = itCedulaEmisor->second.total_facturas + pdata.total_facturas;
                                    itCedulaEmisor->second.total_impuestos = itCedulaEmisor->second.total_impuestos + pdata.total_impuestos;
                                }
                            }else{
                                string strTmp;
                                strTmp.append("Clave: ");
                                strTmp.append(clave->GetText());
                                strTmp.append(" Sin datos de faturacion");
                                errsLog->push_back(strTmp);
                            } //data ok
                        }//fecha ok
                    }else{
                        //cedula no listada como trusted
                        //errsLog->push_back(std::string(cedulaReceptor->GetText()).append(" Cedula no trusted "));
                    } //cedula receptor   
                } 
                //agrega la clave como procesada
                claves->insert(clave->GetText());
            }else{
                errsLog->push_back(std::string(clave->GetText()).append(" WARN respuesta dup "));
            } //if(it == claves->end())
        }
        //std::cout << " facturaElectronica ";
}  

static void process_nota_credito_msg(
    XMLElement * notaCreditoElectronica, 
    std::set<std::string> * cedulas,
    std::set<std::string> * claves,
    std::map<std::string,PROV_DATA> * prov,
    std::list<std::string> * errsLog
    ) {
     //std::cout << " nota credito ";
}  


static void process_file(
    std::string  fileName, 
    std::set<std::string> * cedulas,
    std::set<std::string> * claves,
    std::map<std::string,PROV_DATA> * prov,
    std::list<std::string> * errsLog
    ) {
    ///std::cout << "Processing: "<< fileName;
    XMLDocument doc;
    
    // Load the XML file into the Doc instance
    doc.LoadFile(fileName.c_str());
    if (doc.Error()){
        /*error occurred*/
        fileName.append("  ");
        errsLog->push_back(fileName.append( doc.ErrorStr()));
    } else {
        
        
        // Get root Element
        
        XMLElement * pRootElement = doc.RootElement();
        
       
        
        if (NULL != pRootElement) {
            
               
            //Get facturaElectronica response
    	   			
            if ( !std::string(pRootElement->Value()).compare("MensajeHacienda") ) {
            	
                if(flagRespuestas){
                	
                   process_hacienda_msg(pRootElement,cedulas,claves,prov,errsLog);
                }
            }
           // exit(1);
            //get factura
            if (!std::string(pRootElement->Value()).compare("FacturaElectronica") ||
                !std::string(pRootElement->Value()).compare("TiqueteElectronico")
            ) {
            	
            	
                if(flagFacturas){
                   
                    process_factura_msg(pRootElement,cedulas,claves,prov,errsLog);
                    
                    
                }
            }

            //get nota de credito
            if (!std::string(pRootElement->Value()).compare("NotaCreditoElectronica")) {
                //process_nota_credito_msg(pRootElement,cedulas,claves,prov,errsLog);
            }
            
        }
    
    }

   // std::cout << " Done "<<endl; 
}


static void process_log_file(
    std::list<std::string> * logFile
) {
    
    for(std::list<std::string>::iterator it=logFile->begin(); it!= logFile->end();++it){
        cout<< "ERR: "<<*it<<endl;
    }
    cout<< (int)logFile->size() << " errores"<<endl;
}

static void process_files(
    std::list<std::string> * listFiles,
    std::map<std::string,PROV_DATA> * prov,
    std::set<std::string> * cedulas
) {
    
    std::set<std::string> ids;
    std::list<std::string>::iterator it;
    std::cout << "<<<<<<<<<<<<<<<<< Processing files >>>>>>>>>>>>>>>>>>>>>>\n";
    for (it=listFiles->begin(); it!=listFiles->end(); ++it){
        process_file(*it,cedulas,&ids,prov,&errorsLog);
    }
    //process errors log
    if ((int)errorsLog.size()){
       process_log_file(&errorsLog);
    }
    std::cout << "<<<<<<<<<<<<<<<<< "<< listFiles->size() <<" files processed >>>>>>>>>>>>>>>>>>>>>>\n";
    ids.clear();
}

int main ( int argc, char** argv)
{
    if(argc!=4){
        cout <<" modo de uso:"<<endl;
        cout <<" ./proveedores facturas 2022 02 para procesar facturas"<<endl;
        cout <<" ./proveedores responses 2022 02 para procesar respuestas"<<endl;
        exit(0);
    }
    std::string strReportName;
    strReportName.assign("reporte");
    if(!std::string(argv[1]).compare("facturas")){
        flagFacturas = true;
        strReportName.append("_facturas");
    }

    if(!std::string(argv[1]).compare("confirmaciones")){
        flagRespuestas = true;
        strReportName.append("_confirmaciones");
       
    }

    year.assign(argv[2]);
    month.assign(argv[3]);



    //init lista de cedulas
    cedulaSet.insert("3102200635");
    cedulaSet.insert("3101057571");
    cedulaSet.insert("3101028101");

    list_dir(".",&listFiles);
    process_files(&listFiles,&listProveedores,&cedulaSet);
    print_report("reporte.csv",&listProveedores);
    strReportName.append("_");
    strReportName.append(year);
    strReportName.append("_");
    strReportName.append(month);
    strReportName.append(".csv");
    
    if((int)listProveedores.size()){
    	create_report_detalle(strReportName,&listProveedores);
    }else{
        if(flagFacturas){
           cout << "No se encontraron facturas para "<<year<<" "<<month<<"\n";
        }else{
          cout << "No se encontraron confirmaciones de hacienda para "<<year<<" "<<month<<"\n";
        }
    }
    
    listFiles.clear();
    cedulaSet.clear();
    listProveedores.clear();
    return 0;
}

