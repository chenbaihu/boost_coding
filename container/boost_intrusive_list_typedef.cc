#include <vector>
#include <string>
#include <sstream>

#include <tr1/unordered_set>
#include <tr1/functional>
#include <tr1/memory>

#include <boost/shared_ptr.hpp>
#include <boost/intrusive/list.hpp>

struct DocID16
{   
    uint64_t low_;
    uint64_t high_;

    DocID16(){
        low_     = 0;
        high_    = 0;
    }
};


class DocID : public boost::intrusive::list_base_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> > 
{
    public:
        DocID16   docid_;
};
typedef boost::shared_ptr<DocID> DocIDPtr;
typedef boost::intrusive::list<DocID, boost::intrusive::constant_time_size<false> > DocIDList;  


#define MAX_TEST 1000

int main(int argc,char *argv[])
{
    std::vector<DocIDPtr> docidvec;

    DocIDList docidlist;
    size_t index = 0;
    for(index = 1; index < MAX_TEST; index++){
        DocIDPtr docid_ptr;
        docid_ptr.reset(new DocID());
        docid_ptr->docid_.high_ = index;
        docid_ptr->docid_.low_  = index;
        
        docidvec.push_back(docid_ptr);                      //不保留这个节点，这个节点将被释放
        
        docidlist.push_back(*docid_ptr);
    }
    
    std::cout << "docidvec.size=" << docidvec.size() << std::endl;
    std::cout << "docidlist.size=" << docidlist.size() << std::endl;
    docidvec.clear();                                                     
    
    std::cout << "docidvec.size=" << docidvec.size() << std::endl;
    std::cout << "docidlist.size=" << docidlist.size() << std::endl;
    return 0;
}
 
