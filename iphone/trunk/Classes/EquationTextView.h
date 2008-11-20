//
//  EquationTextView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EquationAbstractView.h"


@interface EquationTextView : EquationAbstractView {

    UILabel     * text;
}

- (id)initWithText:(NSString*)t;

@end
