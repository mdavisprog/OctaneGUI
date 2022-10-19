use std::collections::HashMap;

#[allow(dead_code)]
pub struct Utility {}

impl Utility {
    pub fn map_attributes(attributes: &Vec<xml::attribute::OwnedAttribute>) -> HashMap<String, String> {
        let mut result = HashMap::<String, String>::new();

        for attribute in attributes {
            result.insert(attribute.name.local_name.clone(), attribute.value.clone());
        }

        result
    }
}
