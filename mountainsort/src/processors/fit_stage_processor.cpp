/******************************************************
** See the accompanying README and LICENSE files
** Author(s): Jeremy Magland
** Created: 3/25/2016
*******************************************************/

#include "fit_stage_processor.h"
#include "fit_stage.h"

class fit_stage_ProcessorPrivate
{
public:
    fit_stage_Processor *q;
};

fit_stage_Processor::fit_stage_Processor() {
    d=new fit_stage_ProcessorPrivate;
    d->q=this;

    this->setName("fit_stage");
    this->setVersion("0.1");
    this->setInputFileParameters("timeseries","firings");
    this->setOutputFileParameters("firings_out");
    this->setRequiredParameters("clip_size");
    //this->setOptionalParameters();
}

fit_stage_Processor::~fit_stage_Processor() {
    delete d;
}

bool fit_stage_Processor::check(const QMap<QString, QVariant> &params)
{
    if (!this->checkParameters(params)) return false;
    return true;
}

bool fit_stage_Processor::run(const QMap<QString, QVariant> &params)
{
    QString timeseries_path=params["timeseries"].toString();
    QString firings_path=params["firings"].toString();
    QString firings_out_path=params["firings_out"].toString();
    int clip_size=params["clip_size"].toInt();
    fit_stage_opts opts;
    opts.clip_size=clip_size;
    return fit_stage(timeseries_path,firings_path,firings_out_path,opts);
}
